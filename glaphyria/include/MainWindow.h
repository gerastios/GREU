#pragma once

#include "greuui/include/MainWindow.h"

namespace greu::glaphyria {

class MainWindow: public ui::MainWindow {

public:
    MainWindow();

private:
    virtual void setupUiInt() override;
    struct Detail;
    core::DetailPtr<Detail> d;

};

}
