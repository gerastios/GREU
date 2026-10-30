#include "ImageTypeSelector.h"
#include <QObject>

namespace greu::id3 {

uint const ImageTypeSelector::fileIcon = 1;
uint const ImageTypeSelector::fileOtherIcon = 2;
uint const ImageTypeSelector::frontCover = 3;

ImageTypeSelector *ImageTypeSelector::instance() {
    static ImageTypeSelector instance;
    return &instance;
}

ImageTypeSelector::ImageTypeSelector()
    : core::Selector() {
    allowDeletion(false);
    allowInsertion(false);
    allowUserValues(false);
    insertInit({0, QObject::tr("Other image")});
    insertInit({fileIcon, QObject::tr("File icon")});
    insertInit({fileOtherIcon, QObject::tr("Other file icon")});
    insertInit({frontCover, QObject::tr("Front cover")});
    insertInit({4, QObject::tr("Back cover")});
    insertInit({5, QObject::tr("Leaflet page")});
    insertInit({6, QObject::tr("Media (e.g. label side of CD)")});
    insertInit({7, QObject::tr("Lead artist/lead performer/soloist")});
    insertInit({8, QObject::tr("Artist/performer")});
    insertInit({9, QObject::tr("Conductor")});
    insertInit({10, QObject::tr("Band/orchestra")});
    insertInit({11, QObject::tr("Composer")});
    insertInit({12, QObject::tr("Lyricist/text writer")});
    insertInit({13, QObject::tr("Recording Location")});
    insertInit({14, QObject::tr("During recording")});
    insertInit({15, QObject::tr("During performance")});
    insertInit({16, QObject::tr("Movie/video screen capture")});
    insertInit({17, QObject::tr("A bright coloured fish")});
    insertInit({18, QObject::tr("Illustration")});
    insertInit({19, QObject::tr("Band/artist logotype")});
    insertInit({20, QObject::tr("Publisher/studio logotype")});
}

} // namespace greu::id3
