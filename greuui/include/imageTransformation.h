#pragma once
#include <QImage>
#include "greucore/include/greuGlobal.h"

namespace greu::ui {

enum class GREU_PUBLIC(UI) ImageOrientation {
    portrait,
    landscape,
};

QImage GREU_PUBLIC(UI) scale(QImage const &,
                             int width,
                             int height,
                             QColor const &,
                             ImageOrientation,
                             Qt::TransformationMode = Qt::SmoothTransformation);
QImage GREU_PUBLIC(UI) scale(QImage const &,
                             int width,
                             int height,
                             QWidget const &,
                             ImageOrientation,
                             Qt::TransformationMode = Qt::SmoothTransformation);

} // namespace greu::ui
