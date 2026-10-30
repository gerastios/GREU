#pragma once
#include "greuui/include/MainWindow.h"

class QPushButton;

namespace greu::ui {
class Action;
}
namespace greu::gaion {

class PresentationWidget;

class GREU_PUBLIC(GAION) MainWindow : public ui::MainWindow {
public:
    using ui::MainWindow::MainWindow;

    virtual void setMediumWidget(PresentationWidget*) = 0;
    virtual PresentationWidget* mediumWidget() const = 0;

};

} // namespace greu::gaion
