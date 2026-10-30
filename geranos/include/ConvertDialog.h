#pragma once

#include "Media.h"
#include "greuui/include/Dialog.h"

namespace greu::geranos {

class ConvertDialog : public ui::Dialog {
public:
    ConvertDialog(Media, Qt::WindowFlags = Qt::WindowFlags());
    ~ConvertDialog();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::geranos
