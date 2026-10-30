#include "Application.h"

namespace {
QString const name{"glayphyria"};
QString const version{"0.2.1"};
QString const title{"Γλαφυρία"};
} // namespace

namespace greu::glaphyria {

QString const &Application::appName() {
    return name;
}

QString const &Application::appVersion() {
    return version;
}

QString const &Application::winTitle() {
    return title;
}

} // namespace greu::glaphyria
