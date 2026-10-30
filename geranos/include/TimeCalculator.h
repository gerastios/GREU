#pragma once

#include "greucore/include/DetailPtr.h"
#include "greuui/include/Dialog.h"

namespace greu::geranos {

class TimeCalculator : public ui::Dialog {
public:
    explicit TimeCalculator(Qt::WindowFlags = Qt::WindowFlags());

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::geranos
