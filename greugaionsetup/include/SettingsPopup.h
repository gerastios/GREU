#pragma once
#include "greuui/include/Dialog.h"

namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) SettingsPopup : public ui::Dialog {
public:
    SettingsPopup();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion::setup
