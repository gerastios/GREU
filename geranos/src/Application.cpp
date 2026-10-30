#include "Application.h"

namespace {
QString const name{"geranos"};
QString const version{"0.2.0"};
QString const title{"Γέρανος"};
} // namespace

namespace greu::geranos {

Application::Application(int &argc, char **argv)
    : ui::Application(argc, argv) {}

QString const &Application::appName() {
    return name;
}

QString const &Application::appVersion() {
    return version;
}

QString const &Application::winTitle() {
    return title;
}

} // namespace greu::geranos
