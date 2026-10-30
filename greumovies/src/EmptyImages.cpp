#include "EmptyImages.h"
#include <QImage>
#include <QPixmap>
#include <memory>

using namespace std;

namespace {
unique_ptr<QImage> poster;
unique_ptr<QImage> stillImage;
unique_ptr<QImage> person;
} // namespace

namespace greu::movies {

QImage EmptyImages::poster() {
    if (!::poster)
        ::poster = make_unique<QImage>(":/images/images/poster.png");
    return *::poster;
}

QImage EmptyImages::stillImage() {
    if (!::stillImage)
        ::stillImage = make_unique<QImage>(":/images/images/stillimage.png");
    return *::stillImage;
}

QImage EmptyImages::person() {
    if (!::person)
        ::person = make_unique<QImage>(":/images/images/unknown.png");
    return *::person;
}

} // namespace greu::movies
