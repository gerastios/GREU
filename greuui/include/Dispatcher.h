#pragma once
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class MainWindow;
class Actions;

class GREU_PUBLIC(UI) Dispatcher {
public:
    Dispatcher();

    static Dispatcher * instance();

    GREU_NO_COPY(Dispatcher);
    GREU_NO_MOVE(Dispatcher);

    virtual void initialize() = 0;
    inline virtual void shutDown() {}

    virtual bool canClose() = 0;
    inline virtual void saveOtherSettings() {};
    inline virtual void restoreOtherSettings() {};

    inline virtual bool noUiUpdate() const { return false; }

    virtual MainWindow * mainWindow() const = 0;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

}

#define GREU_D (safeCast<Dispatcher>(Dispatcher::instance()))
#define GREU_DC(C) (safeCast<C>(ui::Dispatcher::instance()))
