#include "Directory.h"
#include <QObject>
#include <QString>
#include "File.h"
#include "greucore/include/vectorUtils.h"
#include "greuui/include/Icons.h"

using namespace std;

namespace greu::tag {

using namespace core;
using namespace ui;

struct Directory::Detail {
    QString const path;
    vector<File *> files;
    FileContainerSig fileAdded;
    FileContainerSig fileRemoved;

    Detail(QString const &path)
        : path(path) {}

    void addFile(File *file, Directory *that) {
        if (contains(files, file))
            return;
        files.push_back(file);
        fileAdded(that, file);
    }

    void removeFile(File *file, Directory *that) {
        if (!contains(files, file))
            return;
        core::erase(files, file);
        fileRemoved(that, file);
    }

    File *at(size_t index) {
        if (index >= files.size())
            return nullptr;
        return *(files.begin() + index);
    }

    optional<size_t> indexOf(File *file) {
        try {
            return core::indexOf(files, file);
        } catch (...) {
            return nullopt;
        }
    }

    bool isModified() {
        auto res = false;
        for (auto &f : files) {
            if ((res = f->isModified()))
                return res;
        }
        return false;
    }
};

Directory::Directory(QString const &path)
    : d(GREU_MKDET(path)) {}

QString const Directory::path() const {
    return d->path;
}

bool Directory::isModified() const {
    return d->isModified();
}

void Directory::addFile(File *file) {
    d->addFile(file, this);
}

void Directory::removeFile(File *file) {
    d->removeFile(file, this);
}

size_t Directory::size() const {
    return d->files.size();
}

File *Directory::at(size_t index) {
    return d->at(index);
}

optional<size_t> Directory::indexOf(File *file) {
    return d->indexOf(file);
}

void Directory::sort(uint typeId, Qt::SortOrder order) const {
    tag::sort(d->files.begin(), d->files.end(), typeId, order);
}

QString const Directory::caption() const {
    return QObject::tr("Directory %1").arg(d->path);
}

QIcon const Directory::tagIcon() const {
    return GREU_ICON(tag_folder);
}

QString const Directory::tagText() const {
    return QObject::tr("folder");
}

FileContainer::FileContainerSig &Directory::fileRemoved() const {
    return d->fileRemoved;
}

FileContainer::FileContainerSig &Directory::fileAdded() const {
    return d->fileAdded;
}

FileContainer::Iterator Directory::begin() {
    return d->files.begin();
}

FileContainer::Iterator Directory::end() {
    return d->files.end();
}

FileContainer::ConstIterator Directory::cbegin() {
    return d->files.cbegin();
}

FileContainer::ConstIterator Directory::cend() {
    return d->files.cend();
}

void Directory::cforEach(ReaderFunction function) const {
    for (auto f : d->files)
        function(f);
}

void Directory::forEach(WriterFunction function) {
    for (auto f : d->files)
        function(f);
}

} // namespace greu::tag
