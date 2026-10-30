#include "FileCreator.h"
#include <QFileInfo>
#include "File.h"

using namespace std;

namespace greu::id3 {

FileCreator::FileCreator() {}

bool FileCreator::handles(QFileInfo const &fi) const {
    return fi.suffix().toUpper() == "MP3";
}

unique_ptr<tag::File> FileCreator::createFile(QFileInfo const &fi) {
    return make_unique<id3::File>(fi);
}

} // namespace greu::id3
