#pragma once
#include <QMainWindow>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class Settings;

class GREU_PUBLIC(UI) MainWindow : public QMainWindow {
public:
    MainWindow();
    static MainWindow *instance();

    void setupUi();

    inline virtual void endEditing() {};
    inline virtual void setStatusMessage(QString const &) {};

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    virtual void setupUiInt() = 0;
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui

#define GREU_M (safeCast<MainWindow>(MainWindow::instance()))
#define GREU_MC(C) (safeCast<C>(MainWindow::instance()))
