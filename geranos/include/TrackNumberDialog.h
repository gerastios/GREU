#pragma once

#include "greuui/include/Dialog.h"

namespace greu::geranos {

class TrackNumberDialog : public ui::Dialog {
public:
    enum : int {
        TrackNumber = 1,
        Device = 2,
    };

    TrackNumberDialog(int);

    void setupUi();
    int trackNumber() const;
    QString device() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::geranos
