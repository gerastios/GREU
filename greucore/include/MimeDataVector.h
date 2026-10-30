#pragma once

#include <QMimeData>
#include "greuGlobal.h"
#include <vector>

namespace greu::core {

template<typename T>
struct GREU_PUBLIC(CORE) MimeDataVector : public QMimeData {
    std::vector<T> data;
};

} // namespace greu::core
