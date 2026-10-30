#include "AlbumChanger.h"
#include "AudioCollection.h"
#include "File.h"
#include "Tag.h"

using namespace std;

namespace greu::tag {

struct AlbumChanger::Detail {
    Album::Key const newKey;
    vector<File *> files;
    AudioCollection *const audioCollection;
    File *mainFile;

    Detail(Album::Key const newKey, vector<File *> files, AudioCollection *audioCollection)
        : newKey(newKey)
        , files(files)
        , audioCollection(audioCollection) {}

    void addFile(File *file) { files.push_back(file); }

    void execute() {
        for (auto f : files)
            changeFile(f);
    }

    void changeFile(File *file) {
        auto tag = file->tag();
        auto oldKey = Album::Key{.title = tag->albumTitle(), .artist = tag->albumArtist()};
        if (oldKey == newKey && file->album()->key() == newKey)
            return;
        file->album()->removeFile(file);
        tag->setAlbumArtist(newKey.artist);
        tag->setAlbumTitle(newKey.title);
        auto newAlbum = audioCollection->album(newKey);
        if (newAlbum) {
            newAlbum->addFile(file);
        } else {
            auto newAlbum = std::make_unique<Album>(newKey);
            newAlbum->addFile(file);
            audioCollection->addAlbum(std::move(newAlbum));
        }
    }
};

AlbumChanger::AlbumChanger(Album::Key const newKey,
                           vector<File *> files,
                           AudioCollection *audioCollection)
    : d(GREU_MKDET(newKey, files, audioCollection)) {}

void AlbumChanger::addFile(File *file) {
    d->addFile(file);
}

void AlbumChanger::setMainFile(File *file) {
    d->mainFile = file;
}

File *AlbumChanger::mainFile() const {
    return d->mainFile;
}

void AlbumChanger::execute() {
    d->execute();
}

vector<File *> const &AlbumChanger::files() const {
    return d->files;
}

} // namespace greu::tag
