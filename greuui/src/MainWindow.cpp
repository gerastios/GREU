#include "MainWindow.h"
#include <QCloseEvent>
#include <QMessageBox>
#include "Application.h"
#include "Dispatcher.h"
#include "Settings.h"
#include "greucore/include/utils.h" // IWYU pragma: keep

namespace greu::ui {

using namespace core;

struct MainWindow::Detail {
    void closeEvent(QCloseEvent *event, MainWindow *that) {
        that->endEditing();
        if (GREU_D->canClose()) {
            Settings::instance().save(that);
            GREU_D->saveOtherSettings();
            event->accept();
        } else {
            event->ignore();
        }
    }
};

MainWindow::MainWindow()
    : d(GREU_MKDET()) {
    setObjectName(Application::applicationName() + "main_window");
}

MainWindow *MainWindow::instance() {
    return GREU_D->mainWindow();
}

void MainWindow::setupUi() {
    setupUiInt();
    Settings::instance().restore(this);
    GREU_D->restoreOtherSettings();
}

void MainWindow::closeEvent(QCloseEvent *event) {
    d->closeEvent(event, this);
}

} // namespace greu::ui
