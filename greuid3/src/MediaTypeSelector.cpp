#include "MediaTypeSelector.h"
#include <QString>

namespace greu::id3 {

MediaTypeSelector *MediaTypeSelector::instance() {
    static MediaTypeSelector instance;
    return &instance;
}

MediaTypeSelector::MediaTypeSelector()
    : core::PlainSelector("MpegMediatypes", true, true, true) {
    if (size() == 0) {
        insertInit("DIG");
        insertInit("DIG/A");
        insertInit("ANA");
        insertInit("ANA/WAC");
        insertInit("ANA/8CA");
        insertInit("CD");
        insertInit("CD/A");
        insertInit("CD/DD");
        insertInit("CD/AD");
        insertInit("CD/AA");
        insertInit("LD");
        insertInit("TT");
        insertInit("TT/33");
        insertInit("TT/45");
        insertInit("TT/71");
        insertInit("TT/76");
        insertInit("TT/78");
        insertInit("TT/80");
        insertInit("MD");
        insertInit("MD/A");
        insertInit("DAT");
        insertInit("DAT/A");
        insertInit("DAT/1");
        insertInit("DAT/2");
        insertInit("DAT/3");
        insertInit("DAT/4");
        insertInit("DAT/5");
        insertInit("DAT/6");
        insertInit("DCC");
        insertInit("DCC/A");
        insertInit("DVD");
        insertInit("DVD/A");
        insertInit("TV");
        insertInit("TV/PAL");
        insertInit("TV/NTSC");
        insertInit("TV/SECAM");
        insertInit("VID");
        insertInit("VID/PAL");
        insertInit("VID/NTSC");
        insertInit("VID/SECAM");
        insertInit("VID/VHS");
        insertInit("VID/SVHS");
        insertInit("VID/BETA");
        insertInit("RAD");
        insertInit("RAD/FM");
        insertInit("RAD/AM");
        insertInit("RAD/LW");
        insertInit("RAD/MW");
        insertInit("TEL");
        insertInit("TEL/I");
        insertInit("MC");
        insertInit("MC/4");
        insertInit("MC/9");
        insertInit("MC/I");
        insertInit("MC/II");
        insertInit("MC/III");
        insertInit("MC/IV");
        insertInit("REE");
        insertInit("REE/9");
        insertInit("REE/19");
        insertInit("REE/38");
        insertInit("REE/76");
        insertInit("REE/I");
        insertInit("REE/II");
        insertInit("REE/III");
        insertInit("REE/IV");
    }
}

} // namespace greu::id3
