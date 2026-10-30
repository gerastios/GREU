#pragma once
#include "greucore/include/DetailPtr.h"

class QWidget;

namespace greu::ui {

class GREU_PUBLIC(UI) VanishingCursor {
public:
    VanishingCursor(QWidget *);
    ~VanishingCursor();
    void show();
    void hide();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
