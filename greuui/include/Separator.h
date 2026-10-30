#pragma once
#include <QFrame>
#include "greucore/include/greuGlobal.h"

namespace greu::ui {

class GREU_PUBLIC(UI) Separator : public QFrame {
public:
    enum Orientation {
        horizontal,
        vertical,
    };

    Separator(Orientation = vertical);
};

} // namespace greu::ui
