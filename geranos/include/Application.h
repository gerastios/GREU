#pragma once

#include "greuui/include/Application.h"

namespace greu::geranos {

class Application : public ui::Application {
public:
    Application(int &argc, char **argv);

    virtual QString const &appName() override;
    virtual QString const &appVersion() override;
    virtual QString const &winTitle() override;
};

} // namespace greu::geranos
