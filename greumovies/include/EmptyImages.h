#pragma once
#include "greucore/include/greuGlobal.h"

class QImage;

namespace greu::movies {

class GREU_PUBLIC(movies) EmptyImages {
public:
    EmptyImages() = delete;

    static QImage poster();
    static QImage stillImage();
    static QImage person();
};

} // namespace greu::movies
