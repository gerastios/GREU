#pragma once

#include "greuui/include/Dialog.h"

namespace greu::grylizo {

class SettingsDialog : public ui::Dialog {
public:
    SettingsDialog();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
