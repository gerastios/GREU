#include "Album.h"
#include <QIcon>
#include "File.h"
#include "Tag.h"
#include "greucore/include/vectorUtils.h"
#include "greuui/include/Icons.h"

using namespace std;

namespace greu::tag {

using namespace core;
using namespace ui;

struct Album::Detail {
    Key key;
    QString artistSort, titleSort;
    QImage image;
    vector<File *> files;
    FileContainerSig fileAdded, fileRemoved;

    Detail(Key const &key)
        : key(key) {}

    bool isIncomplete() const { return key.artist.isEmpty() || key.title.isEmpty(); }

    void determineImage() {
        image = QImage();
        for (auto &f : files) {
            determineImage(f);
            if (!image.isNull())
                return;
        }
    }

    void addFile(File *file, Album *that) {
        if (core::contains(files, file))
            return;
        files.push_back(file);
        auto tag = file->tag();
        extractAttribute(tag->attributeAlbumArtist(), key.artist);
        extractAttribute(tag->attributeAlbumTitle(), key.title);
        extractAttribute(tag->attributeAlbumArtistSort(), artistSort);
        extractAttribute(tag->attributeAlbumTitleSort(), titleSort);
        if (image.isNull())
            determineImage(file);
        file->setAlbum(that);
        fileAdded(that, file);
    }

    void removeFile(File *file, Album *that) {
        core::erase(files, file);
        fileRemoved(that, file);
    }

    File *at(size_t index) {
        if (index >= files.size())
            return nullptr;
        return files.at(index);
    }

    optional<size_t> indexOf(File *file) {
        try {
            return core::indexOf(files, file);
        } catch (...) {
            return nullopt;
        }
    }

    void extractAttribute(PlainAttribute<QString> *attribute, QString &field) {
        if (attribute && field.isEmpty())
            field = attribute->value();
    }

    void determineImage(File *file) {
        auto img = file->tag()->attributeImages();
        if (img) {
            auto image = img->mainValue();
            if (!image.isNull())
                this->image = image;
        }
    }
};

Album::Key Album::buildKey(Tag const *tag) {
    Key res;
    auto a = tag->attributeAlbumArtist();
    if (!a)
        res.artist = QObject::tr("Unknown Artist");
    else
        res.artist = a->value();
    auto t = tag->attributeAlbumTitle();
    if (!t)
        res.title = QObject::tr("Unkown Album");
    else
        res.title = t->value();
    return res;
}

Album::Album(Key const &key)
    : d(GREU_MKDET(key)) {}

QString const Album::artist() const {
    return d->key.artist;
}

QString const Album::title() const {
    return d->key.title;
}

QString const Album::artistSort() const {
    return d->artistSort;
}

QString const Album::titleSort() const {
    return d->titleSort;
}

Album::Key const Album::key() const {
    return d->key;
}

QImage const Album::image() const {
    return d->image;
}

size_t Album::trackCount() const {
    return d->files.size();
}

bool Album::isIncomplete() const {
    return d->isIncomplete();
}

void Album::determineImage() {
    d->determineImage();
}

void Album::addFile(File *file) {
    d->addFile(file, this);
}

void Album::removeFile(File *file) {
    d->removeFile(file, this);
}

size_t Album::size() const {
    return d->files.size();
}

File *Album::at(size_t index) {
    return d->at(index);
}

void Album::sort(uint typeId, Qt::SortOrder order) const {
    tag::sort(d->files.begin(), d->files.end(), typeId, order);
}

optional<size_t> Album::indexOf(File *file) {
    return d->indexOf(file);
}

QString const Album::caption() const {
    return QObject::tr("Album %1 / %2").arg(d->key.title, d->key.artist);
}

QIcon const Album::tagIcon() const {
    return GREU_ICON(tag_album);
}

QString const Album::tagText() const {
    return QObject::tr("album");
}

FileContainer::FileContainerSig &Album::fileRemoved() const {
    return d->fileRemoved;
}

FileContainer::FileContainerSig &Album::fileAdded() const {
    return d->fileAdded;
}

FileContainer::Iterator Album::begin() {
    return d->files.begin();
}

FileContainer::Iterator Album::end() {
    return d->files.end();
}

FileContainer::ConstIterator Album::cbegin() {
    return d->files.cbegin();
}

FileContainer::ConstIterator Album::cend() {
    return d->files.cend();
}

void Album::cforEach(ReaderFunction function) const {
    for (auto f : d->files) {
        function(f);
    }
}

void Album::forEach(WriterFunction function) {
    for (auto f : d->files) {
        function(f);
    }
}

} // namespace greu::tag
