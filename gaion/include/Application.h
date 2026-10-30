#pragma once
#include "greucore/include/DetailPtr.h"
#include "greuui/include/Application.h"

class Application : public greu::ui::Application {
public:
    Application(int &argc, char **argv);

    int run();

    virtual QString const &appName() override;
    virtual QString const &appVersion() override;
    virtual QString const &winTitle() override;

private:
    struct Detail;
    greu::core::DetailPtr<Detail> d;
};
