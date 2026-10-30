#pragma once

#include "greuui/include/Dispatcher.h"

namespace greu::glaphyria {

class Dispatcher : public ui::Dispatcher {
public:
    Dispatcher();

    virtual void initialize() override;
    virtual bool canClose() override;
    virtual ui::MainWindow *mainWindow() const override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::glaphyria
