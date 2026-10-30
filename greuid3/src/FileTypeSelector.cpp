#include "FileTypeSelector.h"
#include <QString>

namespace greu::id3 {

FileTypeSelector *FileTypeSelector::instance() {
    static FileTypeSelector instance;
    return &instance;
}

FileTypeSelector::FileTypeSelector()
    : core::PlainSelector("MpegFileTypes", true, true, true) {
    if (size() == 0) {
        insertInit("VQF");
        insertInit("PCM");
        insertInit("MPG/AAC");
        insertInit("MPG/1");
        insertInit("MPG/2");
        insertInit("MPG/2.5");
        insertInit("MPG/3");
        insertInit("MIME");
    }
}

} // namespace greu::id3
