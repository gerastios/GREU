#pragma once

#include "greuui/include/Dispatcher.h"

namespace greu::geranos {

class Dispatcher : public ui::Dispatcher {
public:
    Dispatcher();

    QString path() const;

    virtual void initialize() override;
    inline virtual bool canClose() override { return true; }
    virtual ui::MainWindow *mainWindow() const override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::geranos
