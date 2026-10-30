#include "File.h"
#include <QDir>
#include <QFileInfo>
#include <QSaveFile>
#include "Tag.h"
#include "greucore/include/FileException.h"
#include "greucore/include/Log.h"
#include <filesystem>

using namespace std;
using namespace greu::core;
using namespace greu::tag;

namespace {

template<typename T>
int compareSafe(T const &fst, T const &snd, File const *fstFile, File const *sndFile) {
    if (fst == snd)
        return fstFile->info().absoluteFilePath() < sndFile->info().absoluteFilePath() ? -1 : 1;
    return fst < snd ? -1 : 1;
}

} // namespace

namespace greu::tag {

struct File::Detail {
    QFileInfo info;
    Album *album;
    Directory *directory;
    Log log;
    ScopedConnection modConnection;

    Detail(QFileInfo const &info)
        : info(info) {}

    void analyze(File *that) {
        QFile physFile(info.filePath());
        if (!physFile.open(QIODevice::ReadOnly)) {
            throw FileException(physFile.errorString(), that->info());
        }
        that->analyzeFile(&physFile);
        auto tag = that->tag();
        tag->setAttributeFileName(
            make_unique<AttributeTypes::PlainStringAttribute>(AttributeTypes::type(
                                                                  AttributeTypes::fileName()),
                                                              that->tag()));
        tag->setFileName(info.completeBaseName());
        tag->setAttributeFileExtension(
            make_unique<AttributeTypes::PlainStringAttribute>(AttributeTypes::type(
                                                                  AttributeTypes::fileExtension()),
                                                              that->tag()));
        tag->setFileExtension(info.suffix());
        tag->setAttributeFilePath(
            make_unique<AttributeTypes::PlainStringAttribute>(AttributeTypes::type(
                                                                  AttributeTypes::filePath()),
                                                              that->tag()));
        tag->setFilePath(info.path());
        modConnection = tag->modifiedChanged().connect([that](bool c) { that->setModified(c); });
    }

    void setTrackNumberByFileName(File *that) {
        auto tn = that->tag()->attributeTrackNumber();
        if (!tn)
            return;
        if (tn->value() != 0)
            return;
        auto fn = that->tag()->fileName().simplified();
        bool ok = false;
        int n;
        for (int len = fn.length(); len > 0 && !ok; len--) {
            n = fn.left(len).toInt(&ok);
        }
        if (ok) {
            tn->setValue(n);
        }
    }

    bool save(File *that) {
        checkFileWriteable(that);
        auto tag = that->tag();
        if (tag->attributeFileName()->isModified()) {
            if (info.fileName() != tag->completeFileName()) {
                QFileInfo nf(QDir(tag->filePath()), tag->completeFileName());
                if (nf.exists())
                    throw FileException(QObject::tr("File already exists"), that->info());
                if (!QFile::rename(info.filePath(), nf.filePath()))
                    throw FileException(QObject::tr("Renaming file not possible"), that->info());
                info = nf;
            }
            tag->attributeFileName()->save();
        }
        auto res = that->saveContent();
        if (res) {
            info.refresh();
            QDir d{info.path()};
            setDirTime(d);
        }
        return res;
    }

    void undoChanges(File *that) {
        auto tag = that->tag();
        for (auto const &a: tag->attributes()) {
            a.second.attribute->undoChanges();
        }
    }

    void checkFileWriteable(File *that) const {
        auto lastDate = info.lastModified();
        auto newInfo = QFileInfo(info.filePath());
        if (!newInfo.isWritable())
            throw FileException(QObject::tr("File is not writeable"), that->info());
        if (newInfo.lastModified() != lastDate)
            throw FileException(QObject::tr("File was externally modified"), that->info());
    }

    void setDirTime(QDir &dir) {
        try {
            filesystem::last_write_time(filesystem::path(dir.absolutePath().toStdWString()),
                                        filesystem::file_time_type::clock::now());
        } catch (filesystem::filesystem_error &) {
            return;
        }
        QFileInfo i{dir.absolutePath()};
        if (i.absolutePath() == QDir::rootPath())
            return;
        QDir u(i.path());
        setDirTime(u);
    }
};

File::File(QFileInfo const &info)
    : d(GREU_MKDET(info)) {}

QFileInfo const &File::info() const {
    return d->info;
}

void File::updateInfo() {
    d->info.refresh();
}

void File::analyze() {
    d->analyze(this);
}

void File::setAlbum(Album *value) {
    d->album = value;
}

Album *File::album() const {
    return d->album;
}

void File::setDirectory(Directory *v) {
    d->directory = v;
}

Directory *File::directory() const {
    return d->directory;
}

bool File::save() {
    return d->save(this);
}

Log &File::log() const {
    return d->log;
}

void sort(vector<File *>::iterator filesBegin,
          vector<File *>::iterator filesEnd,
          uint typeId,
          Qt::SortOrder order) {
    std::sort(filesBegin, filesEnd, [typeId, order](File const *fst, File const *snd) {
        int res = 0;
        if (!fst)
            return true;
        if (!snd)
            return false;
        auto fa = fst->tag()->attribute(typeId);
        auto sa = snd->tag()->attribute(typeId);
        auto fas = dynamic_cast<AttributeTypes::PlainStringAttribute *>(fa);
        auto sas = dynamic_cast<AttributeTypes::PlainStringAttribute *>(sa);
        if (fas && sas) {
            res = compareSafe(fas->value().compare(sas->value(),
                                                   Qt::CaseSensitivity::CaseInsensitive),
                              0,
                              fst,
                              snd);
        } else {
            auto fau = dynamic_cast<AttributeTypes::PlainUintAttribute *>(fa);
            auto sau = dynamic_cast<AttributeTypes::PlainUintAttribute *>(sa);
            if (fau && sau) {
                if (typeId == AttributeTypes::trackNumber()) {
                    auto fdn = fst->tag()->attributeDiscNumber();
                    auto sdn = snd->tag()->attributeDiscNumber();
                    if (fdn && sdn) {
                        if (fdn->value() == sdn->value())
                            res = compareSafe(fau->value(), sau->value(), fst, snd);
                        else
                            res = compareSafe(fdn->value(), sdn->value(), fst, snd);
                    }
                } else {
                    res = compareSafe(fau->value(), sau->value(), fst, snd);
                }
            } else {
                auto fat = dynamic_cast<AttributeTypes::PlainTimeAttribute *>(fa);
                auto sat = dynamic_cast<AttributeTypes::PlainTimeAttribute *>(sa);
                if (fat && sat) {
                    res = compareSafe(fat->value(), sat->value(), fst, snd);
                } else {
                    assert("undefined attribute");
                }
            }
        }
        if (res == 0)
            res = compareSafe(0, 0, fst, snd);
        if (order == Qt::SortOrder::DescendingOrder)
            res *= -1;
        return res == -1;
    });
}

} // namespace greu::tag
