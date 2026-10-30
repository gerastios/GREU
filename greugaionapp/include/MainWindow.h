#pragma once
#include "greugaion/include/MainWindow.h"
#include <boost/signals2.hpp>

namespace greu::gaion::app {

class GREU_PUBLIC(GAIONAPP) MainWindow : public gaion::MainWindow {
public:
    MainWindow();

    virtual void setMediumWidget(PresentationWidget *) override;
    virtual PresentationWidget *mediumWidget() const override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
    virtual void setupUiInt() override;

};
} // namespace greu::gaion::app
