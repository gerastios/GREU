#pragma once
#include "Album.h"
#include <vector>

namespace greu::tag {

class file;
class AudioCollection;

class GREU_PUBLIC(TAG) AlbumChanger {
public:
    AlbumChanger(Album::Key const newKey, std::vector<File *>, AudioCollection *);

    void addFile(File *);
    void setMainFile(File *);
    File *mainFile() const;
    void execute();

    std::vector<File *> const &files() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tag
