#pragma once

#include "greuui/include/Application.h"

namespace greu::glaphyria {

class Application : public ui::Application {
public:
    using ui::Application::Application;
    virtual QString const &appName() override;
    virtual QString const &appVersion() override;
    virtual QString const &winTitle() override;
};

} // namespace greu::glaphyria
