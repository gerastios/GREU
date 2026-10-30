#pragma once
#include "Album.h"
#include "FileHolder.h"
#include "greucore/include/ModifiableCollection.h"
#include "greucore/include/signalUtils.h"

class QDir;

namespace greu::core {
class Log;
class Saver;
} // namespace greu::core

namespace greu::tag {

class FileCreator;
struct FileContainer;
class File;
class Directory;

class GREU_PUBLIC(TAG) AudioCollection : public core::ModifiableCollection,
                                         public FileHolder {
public:
    AudioCollection(uint packageSize, uint messageSize);
    virtual ~AudioCollection() = default;

    void initialize(QString const &path, core::Log *log);

    core::SizeSig &counted() const;
    using FileContainerSig = boost::signals2::signal<void(FileContainer *)>;
    FileContainerSig &fileContainerChanged() const;
    using LogSig = boost::signals2::signal<void(core::Log *)>;
    LogSig &logAdded() const;
    using FileSig = boost::signals2::signal<void(File *)>;
    FileSig &aboutToDeleteFile() const;
    using AlbumSig = boost::signals2::signal<void(Album *)>;
    AlbumSig &aboutToDeleteAlbum() const;
    core::EmptySig &albumListChanged() const;
    using DirectorySig = boost::signals2::signal<void(Directory *)>;
    DirectorySig &directoryRefreshed() const;

    virtual size_t size() const override;
    virtual File *at(size_t) override;
    virtual std::optional<size_t> indexOf(File *file) override;

    bool deleteFile(File *file);

    size_t albumCount() const;
    Album *albumAt(size_t) const;
    Album *album(QString const &title, QString const &artist) const;
    Album *album(Album::Key const &key) const;
    void addAlbum(std::unique_ptr<Album> &&album);
    size_t indexOf(Album const *) const;
    enum Sort {
        title,
        artist,
        trackCount,
    };
    void sortAlbums(Qt::SortOrder, Sort);
    void removeAlbum(Album::Key const &key);

    Directory *directory(QString const &path, bool onlyExisting = false);
    size_t directoryCount() const;
    bool dirIsModified(QDir const &dir) const;
    void refreshDir(QDir const &dir);

    size_t modifiedFilesCount() const;
    size_t savedFilesCount() const;
    std::vector<File *> filesToSave() const;
    std::unique_ptr<core::Saver> saver();

    virtual void cforEach(ReaderFunction) const override;
    virtual void forEach(WriterFunction) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tag
