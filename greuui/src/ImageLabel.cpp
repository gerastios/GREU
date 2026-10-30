#include "ImageLabel.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct ImageLabel::Detail {
    QImage image;
};

ImageLabel::ImageLabel()
    : d(GREU_MKDET()) {}

void ImageLabel::setImage(QImage const &img, bool show) {
    d->image = img;
    setSizePolicy(QSizePolicy::MinimumExpanding, QSizePolicy::Fixed);
    setMinimumSize(img.width(), img.height());
    if(show)
        showImage();
}

QImage const &ImageLabel::image() const {
    return d->image;
}

void ImageLabel::showImage() {
    setPixmap(QPixmap::fromImage(d->image));
}

} // namespace greu::ui
