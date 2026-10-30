#include "ImageContainer.h"
#include <QBuffer>
#include <QByteArray>
#include <QImage>
#include <QString>

using namespace std;

namespace {
constexpr string_view bmp{"BMP"};
constexpr string_view gif{"GIF"};
constexpr string_view jpg{"JPG"};
constexpr string_view png{"PNG"};
constexpr string_view pbm{"PBM"};
constexpr string_view pgm{"PGM"};
constexpr string_view ppm{"PPM"};
constexpr string_view xbm{"XBM"};
constexpr string_view xpm{"XPM"};
constexpr string_view svg{"SVG"};
constexpr string_view tiff{"TIFF"};
constexpr string_view webp{"WEBP"};
} // namespace

namespace greu::ui {

using namespace core;

struct ImageContainer::Detail {
    QByteArray data;
    Type type;

    Detail()
        : type(Unknown) {}
    Detail(QByteArray const &d, Type t)
        : data(d)
        , type(t) {}

    char const *format() const {
        switch (type) {
        case WindowsBitmap:
            return bmp.data();
        case GraphicInterchangeFormat:
            return gif.data();
        case JointPhotographicExpertsGroup:
            return jpg.data();
        case PortableNetworkGraphics:
            return png.data();
        case PortableBitmap:
            return pbm.data();
        case PortableGraymap:
            return pgm.data();
        case PortablePixmap:
            return ppm.data();
        case X11Bitmap:
            return xbm.data();
        case X11Pixmap:
            return xpm.data();
        case ScalableVectorGraphics:
            return svg.data();
        case TaggedImageFileFormat:
            return tiff.data();
        case WebP:
            return webp.data();
        default:
            return nullptr;
        }
    }
};

ImageContainer::ImageContainer()
    : d(GREU_MKDET()) {}

ImageContainer::ImageContainer(QByteArray const &d, Type t)
    : d(GREU_MKDET(d, t)) {}

ImageContainer::ImageContainer(ImageContainer const &o)
    : d(GREU_MKDET(o.d->data, o.d->type)) {}

ImageContainer ImageContainer::fromImage(QImage const &img) {
    ImageContainer res;
    QBuffer buf(&res.d->data);
    buf.open(QIODevice::WriteOnly);
    img.save(&buf, "JPG", 100);
    res.d->type = JointPhotographicExpertsGroup;
    return res;
}

ImageContainer &ImageContainer::operator=(ImageContainer const &o) {
    d->data = o.d->data;
    d->type = o.d->type;
    return *this;
}

void ImageContainer::setImage(QByteArray const &data, Type t) {
    d->data = data;
    d->type = t;
}

bool ImageContainer::isValid() const {
    return d->type != Unknown;
}

bool ImageContainer::isEmpty() const {
    return d->data.isEmpty();
}

ImageContainer::Type ImageContainer::type() const {
    return d->type;
}

QByteArray const &ImageContainer::data() const {
    return d->data;
}

QImage ImageContainer::toImage() const {
    if (d->type == Unknown || d->data.isEmpty()) {
        auto res = QImage(10, 10, QImage::Format_ARGB32);
        res.fill(0);
        return res;
    }
    return QImage::fromData(d->data, d->format());
}

ImageContainer::Type ImageContainer::fromExension(QString const &s) {
    auto u = s.toUpper();
    if (u == "BMP")
        return WindowsBitmap;
    if (u == "GIF")
        return GraphicInterchangeFormat;
    if (u == "JPG" || u == "JPEG" || u == "JFIF" || u == "PJPEG" || u == "PJP")
        return JointPhotographicExpertsGroup;
    if (u == "PNG")
        return PortableNetworkGraphics;
    if (u == "PBM")
        return PortableBitmap;
    if (u == "PGM")
        return PortableGraymap;
    if (u == "PPM")
        return PortablePixmap;
    if (u == "XBM")
        return X11Bitmap;
    if (u == "XPM")
        return X11Pixmap;
    if (u == "SVG")
        return ScalableVectorGraphics;
    if (u == "TIFF" || u == "TIF")
        return TaggedImageFileFormat;
    if (u == "WEBP")
        return WebP;
    return Unknown;
}

} // namespace greu::ui
