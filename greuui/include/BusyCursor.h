#pragma once
#include "greucore/include/DetailPtr.h"

class QWidget;

namespace greu::ui {

class GREU_PUBLIC(UI) BusyCursor {
public:
    BusyCursor();
    ~BusyCursor();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
