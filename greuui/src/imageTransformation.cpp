#include "imageTransformation.h"
#include <QPainter>
#include <QWidget>

using namespace std;

namespace greu::ui {

QImage scale(QImage const &source,
             int width,
             int height,
             QColor const &color,
             ImageOrientation orientation,
             Qt::TransformationMode mode) {
    auto s = source.size();
    if ((orientation == ImageOrientation::landscape && width < height)
        || (orientation == ImageOrientation::portrait && width > height))
        swap(width, height);
    if (s.height() == height && s.width() == width)
        return source;
    auto sw = s.width();
    auto sh = s.height();
    auto s1 = sw * height;
    auto s2 = width * sh;
    if (s1 == s2) // same aspect ratio?
        return source.scaled(width, height, Qt::KeepAspectRatio, mode);
    QImage res{width, height, source.format()};
    res.fill(color);
    QPainter paint(&res);
    if (s1 < s2) { // add left and right
        auto w = sw * height / sh;
        paint.drawImage(QPoint(abs(width - w) / 2, 0),
                        source.scaled(w, height, Qt::KeepAspectRatio, mode));
    } else { // add top and bottom
        auto h = sh * width / sw;
        paint.drawImage(QPoint(0, (abs(height - h)) / 2),
                        source.scaled(width, h, Qt::KeepAspectRatio, mode));
    }
    return res;
}

QImage scale(QImage const &source,
             int width,
             int height,
             QWidget const &widget,
             ImageOrientation orientation,
             Qt::TransformationMode mode) {
    return scale(source, width, height, widget.palette().color(QPalette::Window), orientation, mode);
}

} // namespace greu::ui
