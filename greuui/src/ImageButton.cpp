#include "ImageButton.h"
#include "connect.h"
#include <functional>

using namespace std;

namespace greu::ui {

using namespace core;

struct ImageButton::Detail {
    StringSig clicked;
    QString target;

    void setText(QString const &tgt, ImageButton *that) {
        target = tgt;
        if (that->image().isNull() || target.isEmpty())
            that->setText("");
        else
            that->setText(QString("<a href=\"%1\"><img src=\"pics://image\"></a>").arg(target));
    }

    QVariant getImage(QUrl const &, ImageButton *that) { return QVariant(that->image()); }
};

ImageButton::ImageButton()
    : d(GREU_MKDET()) {
    setOpenExternalLinks(false);
    setResourceProvider(std::bind(&Detail::getImage, d.get(), placeholders::_1, this));
    ui::connect(this, [this](auto) { d->clicked(d->target); });
}

void ImageButton::setImage(QImage const &img, QString const &target) {
    ImageLabel::setImage(img, false);
    d->setText(target, this);
    if (target.isEmpty())
        showImage();
    else
        repaint();
}

StringSig &ImageButton::clicked() const {
    return d->clicked;
}

} // namespace greu::ui
