#include "Application.h"

namespace {
greu::ui::Application *instance = nullptr;
} // namespace

namespace greu::ui {

using namespace core;

struct Application::Detail {
    Dispatcher *dispatcher;
};

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
    , d(GREU_MKDET()) {
    assert(::instance == nullptr);
    ::instance = this;
    setStyleSheet("QPushButton:disabled, QLineEdit:disabled, QLabel:disabled, QComboBox:disabled, "
                  "QCheckBox::disabled { color: gray }");
}

Application *Application::instance() {
    return ::instance;
}

void Application::setDispatcher(Dispatcher *v) {
    d->dispatcher = v;
}

Dispatcher *Application::dispatcher() const {
    return d->dispatcher;
}

} // namespace greu::ui
