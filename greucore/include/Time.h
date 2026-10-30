#pragma once

#include <QTime>
#include "greuGlobal.h"

namespace greu::core {

class GREU_PUBLIC(CORE) Time : public QTime {
public:
    Time() = default;
    Time(int h, int m, int s = 0, int ms = 0);

    static Time fromSeconds(uint seconds);
};

} // namespace greu::core
