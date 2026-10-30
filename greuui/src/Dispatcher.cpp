#include "Dispatcher.h"
#include "Application.h"
#include "greucore/include/Settings.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct Dispatcher::Detail {
    core::Settings settings;

    Detail()
        : settings(Application::instance()->appName()) {}
};

Dispatcher::Dispatcher()
    : d(GREU_MKDET()) {
    Application::instance()->setDispatcher(this);
}

Dispatcher *Dispatcher::instance() {
    return Application::instance()->dispatcher();
}

} // namespace greu::ui
