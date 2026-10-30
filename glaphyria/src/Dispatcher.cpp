#include "Dispatcher.h"
#include "MainWindow.h"
#include "greucore/include/GuardedPtr.h"

namespace greu::glaphyria {

using namespace core;

struct Dispatcher::Detail {
    GuardedPtr<MainWindow> mainWindow;

    void initialize() {
        mainWindow = new MainWindow;
        mainWindow->setupUi();
        mainWindow->show();
    }
};

Dispatcher::Dispatcher()
    : d(GREU_MKDET()) {}

void Dispatcher::initialize() {
    d->initialize();
}

bool Dispatcher::canClose() {
    return true;
}

ui::MainWindow *Dispatcher::mainWindow() const {
    return d->mainWindow.get();
}

} // namespace greu::glaphyria
