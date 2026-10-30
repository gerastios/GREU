#pragma once

#include "greuGlobal.h"
#include <QStandardPaths>
#include <vector>

namespace greu::core {

class GREU_PUBLIC(CORE) StandardPaths {
public:
    StandardPaths() = delete;

    static QString writeableLocation(QStandardPaths::StandardLocation type);
    static std::vector<QString> rootPaths();

};

}
