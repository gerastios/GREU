#include "AudioCollection.h"
#include <QDir>
#include <QFileInfo>
#include "Album.h"
#include "Directory.h"
#include "File.h"
#include "FileCreator.h"
#include "FileCreatorRegistry.h"
#include "Tag.h"
#include "greucore/include/Exception.h"
#include "greucore/include/Log.h"
#include "greucore/include/Saver.h"
#include "greucore/include/utils.h"
#include "greucore/include/vectorUtils.h"
#include <future>
#include <optional>
#include <unordered_map>

using namespace std;
using namespace greu::tag;
using namespace greu::core;

namespace {

struct FileEntry {
    FileCreator *creator;
    QFileInfo info;
};

using FileEntries = vector<vector<FileEntry>>;

struct Fetcher {
    using Entries = vector<unique_ptr<File>>;

    vector<FileEntry> *fileInfos;
    Log log;
    uint const messagesize;
    SizeSig counted;

    Fetcher(vector<FileEntry> *fileInfos, uint messagesize)
        : fileInfos(fileInfos)
        , messagesize(messagesize) {}

    Entries fetch() {
        size_t count = 0;
        Entries res;
        for (auto &i : *fileInfos) {
            try {
                auto file = i.creator->createFile(i.info);
                file->analyze();
                log.add(file->log());
                res.push_back(std::move(file));
                ++count;
                if (count % messagesize == 0) {
                    counted(count);
                    count = 0;
                }
            } catch (Exception &exc) {
                log.add(exc);
            }
        }
        return res;
    }

    void deliverLog(Log &log) { log.add(this->log); }
};

} // namespace

namespace greu::tag {

struct AudioCollection::Detail {
    struct Saver : public core::Saver {
        AudioCollection::Detail *that;
        AudioCollection *audioCollection;
        size_t saveCnt;

        virtual ~Saver() = default;

        virtual void save() override {
            auto toSave = audioCollection->filesToSave();
            saveCnt = 0;
            if (toSave.size() == 0)
                return;
            for (auto f = toSave.begin(); f != toSave.end() && !isCancelled(); ++f) {
                (*f)->save();
                ++saveCnt;
                that->counted(saveCnt * 100 / toSave.size());
            }
            if (!isCancelled())
                audioCollection->setModified(false);
        }

        virtual void saveAll() override {
            auto toSave = audioCollection->size();
            if (toSave == 0)
                return;
            audioCollection->forEach([this, toSave](File *file) {
                file->save();
                ++saveCnt;
                that->counted(saveCnt * 100 / toSave);
            });
            if (!isCancelled())
                audioCollection->setModified(false);
        }
    };

    struct AlbumEntry {
        unique_ptr<Album> album;
        ScopedConnection removeConnection;
    };
    size_t fetched;
    size_t filesToRead;
    vector<unique_ptr<File>> files;
    unordered_map<Album::Key, AlbumEntry> albums;
    vector<Album *> albumList;
    unordered_map<QString, unique_ptr<Directory>> directories;
    SizeSig counted;
    FileContainerSig fileContainerChanged;
    LogSig logAdded;
    FileSig aboutToDeleteFile;
    AlbumSig aboutToDeleteAlbum;
    EmptySig albumListChanged;
    DirectorySig directoryRefreshed;
    static std::mutex countMutex;
    static std::mutex albumMutex;
    static std::mutex directoryMutex;
    uint const packageSize, messageSize;
    size_t saveCnt;

    Detail(uint packageSize, uint messageSize)
        : packageSize(packageSize)
        , messageSize(messageSize) {}

    void readFiles(QDir const &dir, Log *log, AudioCollection *that) {
        fetched = filesToRead = 0;
        FileEntries files;
        getFiles(files, *log, dir, true);
        if (files.size() == 0)
            return;
        if (files.size() == 1) {
            for (auto &fi : fetchFiles(files[0], log))
                addFile(std::move(fi), *log, that);
        } else {
            vector<future<Fetcher::Entries>> futures;
            for (auto &f : files) {
                future<Fetcher::Entries> fut = async(launch::async,
                                                     std::bind(&Detail::fetchFiles, this, f, log));
                futures.push_back(std::move(fut));
            }
            for (auto &fut : futures) {
                auto files = fut.get();
                for (auto &fi : files)
                    addFile(std::move(fi), *log, that);
            }
        }
    }

    File *at(size_t index) {
        if (index >= files.size())
            return nullptr;
        return files.at(index).get();
    }

    optional<size_t> indexOf(File *file) {
        try {
            auto res = std::find_if(files.begin(), files.end(), [file](auto &f) {
                return f.get() == file;
            });
            if (res == files.end())
                return std::nullopt;
            return res - files.begin();
        } catch (...) {
            return std::nullopt;
        }
    }

    bool deleteFile(File *file, AudioCollection *that, bool delPhysFile = false) {
        auto fc = indexOf(file);
        if (!fc)
            return false;
        aboutToDeleteFile(file);
        auto album = file->album();
        if (album) {
            album->removeFile(file);
            fileContainerChanged(album);
            if (album->size() == 0)
                removeAlbum(album->key());
        }
        auto directory = file->directory();
        if (directory) {
            directory->removeFile(file);
        }
        that->remove(file);
        if (delPhysFile)
            QFile::remove(file->info().filePath());
        return true;
    }

    Album *albumAt(size_t index) const {
        try {
            return albumList[index];
        } catch (...) {
            return nullptr;
        }
    }

    Album *album(QString const &title, QString const &artist) const {
        try {
            auto &res = albums.at(Album::Key{.title = title, .artist = artist});
            return res.album.get();
        } catch (...) {
            return nullptr;
        }
    }

    void addAlbum(unique_ptr<Album> &&album) {
        ScopedConnection conn = album->fileRemoved().connect([this](auto container, File *) {
            auto album = safeCast<Album>(container);
            if (album->size() == 0)
                removeAlbum(album->key());
        });
        albumList.push_back(album.get());
        auto key = album->key();
        albums[key] = AlbumEntry{.album = std::move(album), .removeConnection = std::move(conn)};
    }

    size_t indexOf(Album const *album) const {
        auto it = std::find(albumList.begin(), albumList.end(), album);
        if (it == albumList.end())
            return -1;
        return it - albumList.begin();
    }

    void sortAlbums(Qt::SortOrder order, Sort by) {
        std::sort(albumList.begin(), albumList.end(), [order, by](Album *fst, Album *snd) {
            if (!fst || !snd)
                return false;
            switch (by) {
            case Sort::artist: {
                auto res = fst->artist().compare(snd->artist(), Qt::CaseInsensitive) < 0 ? true
                                                                                         : false;
                if (order == Qt::SortOrder::DescendingOrder)
                    return !res;
                return res;
                ;
            }
            case Sort::title: {
                auto res = fst->title().compare(snd->title(), Qt::CaseInsensitive) < 0 ? true
                                                                                       : false;
                if (order == Qt::SortOrder::DescendingOrder)
                    return !res;
                return res;
                ;
            }
            case Sort::trackCount: {
                auto res = fst->trackCount() < snd->trackCount() ? true : false;
                if (order == Qt::SortOrder::DescendingOrder)
                    return !res;
                return res;
                ;
            }
            }
            return true;
        });
    }

    void removeAlbum(Album::Key const &key) {
        if (albums.contains(key)) {
            aboutToDeleteAlbum(albums[key].album.get());
            core::erase(albumList, albums.at(key).album.get());
            albums[key].removeConnection.disconnect();
            albums.erase(key);
            albumListChanged();
        }
    }

    Directory *directory(QString const &path, bool onlyExisting, AudioCollection *that) {
        if (!directories.contains(path)) {
            if (onlyExisting)
                return nullptr;
            Log log;
            readDirectory(path, &log, that);
            logAdded(&log);
            if (!directories.contains(path))
                return nullptr;
        }
        return directories[path].get();
    }

    bool dirIsModified(QDir const &dir) const {
        if (!directories.contains(dir.canonicalPath()))
            return false;
        bool res = false;
        directories.at(dir.canonicalPath())->cforEach([&res](auto file) {
            if (file->isModified())
                res = true;
        });
        return res;
    }

    void refreshDir(QDir const &dir, AudioCollection *that) {
        if (!directories.contains(dir.canonicalPath())) {
            directory(dir.canonicalPath(), false, that);
            return;
        }
        vector<File *> folderFiles;
        auto d = directories.at(dir.canonicalPath()).get();
        d->forEach([&folderFiles](File *file) { folderFiles.push_back(file); });
        for (auto f : folderFiles) {
            deleteFile(f, that);
        }
        Log log;
        readFiles(dir, &log, that);
        logAdded(&log);
        directoryRefreshed(d);
    }

    vector<File *> filesToSave() const {
        vector<File *> res;
        for (auto &f : files) {
            if (f->isModified())
                res.push_back(f.get());
        }
        return res;
    }

    void getFiles(FileEntries &files, Log &log, QDir const &dir, bool includeSubdirs) {
        auto entries = dir.entryInfoList(QDir::Dirs | QDir::Files | QDir::NoSymLinks | QDir::NoDot
                                         | QDir::NoDotDot | QDir::NoDotAndDotDot | QDir::Writable);
        for (auto &entry : entries) {
            if (entry.isFile()) {
                auto co = FileCreatorRegistry::instance().instanceFor(entry)->handler;
                if (co)
                    addFileEntry(files, co, entry);
                else
                    log.addWarn(QObject::tr("File type not supported: %1").arg(entry.filePath()));
            } else {
                if (entry.isDir() && includeSubdirs)
                    getFiles(files, log, QDir(entry.filePath()), true);
            }
        }
    }

    void addFileEntry(FileEntries &files, FileCreator *creator, QFileInfo const &info) {
        ++filesToRead;
        vector<FileEntry> *fe;
        if (files.size() == 0)
            fe = &files.emplace_back(vector<FileEntry>());
        else if (packageSize == 0 || files[files.size() - 1].size() < packageSize)
            fe = &files[files.size() - 1];
        else
            fe = &files.emplace_back(vector<FileEntry>());
        fe->emplace_back(FileEntry{.creator = creator, .info = info});
    }

    void addFile(unique_ptr<File> &&file, Log &log, AudioCollection *that) {
        auto tag = file->tag();
        if (!hasAttribute(*file.get(), log, tag->attributeAlbumArtist(), "album artist"))
            return;
        if (!hasAttribute(*file.get(), log, tag->attributeAlbumTitle(), "album title"))
            return;
        if (!hasAttribute(*file.get(), log, tag->attributeSongArtist(), "song artist"))
            return;
        if (!hasAttribute(*file.get(), log, tag->attributeSongTitle(), "song title"))
            return;
        if (!hasAttribute(*file.get(), log, tag->attributeTrackNumber(), "track number"))
            return;
        that->add(file.get());
        addFileToAlbum(file.get());
        addFileToDirectory(file.get());
        files.push_back(std::move(file));
    }

    template<typename T>
    bool hasAttribute(File const &file, Log &log, T att, QString const &name) {
        if (!att) {
            log.addWarn(QObject::tr("Does not support %1: %2").arg(name, file.info().fileName()));
            return false;
        }
        return true;
    }

    void addFileToAlbum(File *file) {
        auto tag = file->tag();
        auto key = Album::buildKey(tag);
        if (!albums.contains(key)) {
            lock_guard<std::mutex> g{albumMutex};
            auto album = make_unique<Album>(key);
            addAlbum(std::move(album));
        }
        auto &album = albums.at(key);
        album.album->addFile(file);
        file->setAlbum(album.album.get());
        albumListChanged();
    }

    void addFileToDirectory(File *file) {
        auto key = file->info().path();
        if (!directories.contains(key)) {
            lock_guard<std::mutex> g{directoryMutex};
            auto folder = make_unique<Directory>(key);
            directories[key] = std::move(folder);
        }
        auto &directory = directories.at(key);
        directory->addFile(file);
        file->setDirectory(directory.get());
    }

    void readDirectory(QString const &path, Log *log, AudioCollection *that) {
        QDir dir(path);
        readFiles(dir, log, that);
    }

    Fetcher::Entries fetchFiles(vector<FileEntry> &files, Log *log) {
        Fetcher fetcher{&files, messageSize};
        fetcher.counted.connect([this](auto ctd) {
            if (filesToRead == 0)
                return;
            lock_guard<std::mutex> guard(countMutex);
            fetched += ctd;
            counted(fetched * 100 / filesToRead);
        });
        auto res = fetcher.fetch();
        fetcher.deliverLog(*log);
        return res;
    }
};

std::mutex AudioCollection::Detail::countMutex;
std::mutex AudioCollection::Detail::albumMutex;
std::mutex AudioCollection::Detail::directoryMutex;

AudioCollection::AudioCollection(uint packageSize, uint messageSize)
    : d(GREU_MKDET(packageSize, messageSize)) {}

void AudioCollection::initialize(QString const &path, Log *log) {
    QFileInfo info{path};
    if (!info.isDir())
        info = QFileInfo(info.path());
    d->readFiles(QDir(info.filePath()), log, this);
}

AudioCollection::DirectorySig &AudioCollection::directoryRefreshed() const {
    return d->directoryRefreshed;
}

EmptySig &AudioCollection::albumListChanged() const {
    return d->albumListChanged;
}

AudioCollection::AlbumSig &AudioCollection::aboutToDeleteAlbum() const {
    return d->aboutToDeleteAlbum;
}

AudioCollection::FileSig &AudioCollection::aboutToDeleteFile() const {
    return d->aboutToDeleteFile;
}

AudioCollection::LogSig &AudioCollection::logAdded() const {
    return d->logAdded;
}

AudioCollection::FileContainerSig &AudioCollection::fileContainerChanged() const {
    return d->fileContainerChanged;
}

SizeSig &AudioCollection::counted() const {
    return d->counted;
}

size_t AudioCollection::size() const {
    return d->files.size();
}

File *AudioCollection::at(size_t index) {
    return d->at(index);
}

optional<size_t> AudioCollection::indexOf(File *file) {
    return d->indexOf(file);
}

bool AudioCollection::deleteFile(File *file) {
    return d->deleteFile(file, this);
}

size_t AudioCollection::albumCount() const {
    return d->albums.size();
}

Album *AudioCollection::albumAt(size_t index) const {
    return d->albumAt(index);
}

Album *AudioCollection::album(QString const &title, QString const &artist) const {
    return d->album(title, artist);
}

Album *AudioCollection::album(Album::Key const &key) const {
    return d->album(key.title, key.artist);
}

void AudioCollection::addAlbum(unique_ptr<Album> &&album) {
    d->addAlbum(std::move(album));
}

size_t AudioCollection::indexOf(Album const *album) const {
    return d->indexOf(album);
}

void AudioCollection::sortAlbums(Qt::SortOrder order, Sort by) {
    d->sortAlbums(order, by);
}

void AudioCollection::removeAlbum(Album::Key const &key) {
    d->removeAlbum(key);
}

Directory *AudioCollection::directory(QString const &path, bool onlyExisting) {
    return d->directory(path, onlyExisting, this);
}

size_t AudioCollection::directoryCount() const {
    return d->directories.size();
}

bool AudioCollection::dirIsModified(QDir const &dir) const {
    return d->dirIsModified(dir);
}

void AudioCollection::refreshDir(QDir const &dir) {
    d->refreshDir(dir, this);
}

size_t AudioCollection::modifiedFilesCount() const {
    return d->filesToSave().size();
}

size_t AudioCollection::savedFilesCount() const {
    return d->saveCnt;
}

vector<File *> AudioCollection::filesToSave() const {
    return d->filesToSave();
}

unique_ptr<core::Saver> AudioCollection::saver() {
    auto r = make_unique<Detail::Saver>();
    r->audioCollection = this;
    r->that = d.get();
    return r;
}

void AudioCollection::cforEach(ReaderFunction function) const {
    for (auto &f : d->files) {
        function(f.get());
    }
}

void AudioCollection::forEach(WriterFunction function) {
    for (auto &f : d->files) {
        function(f.get());
    }
}

} // namespace greu::tag
