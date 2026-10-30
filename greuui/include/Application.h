#pragma once
#include <QApplication>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class Dispatcher;

class GREU_PUBLIC(UI) Application : public QApplication {
public:
    Application(int &argc, char **argv);
    virtual ~Application() = default;
    GREU_NO_COPY(Application);
    GREU_NO_MOVE(Application);
    static Application *instance();

    virtual QString const &appName() = 0;
    virtual QString const &appVersion() = 0;
    virtual QString const &winTitle() = 0;
    void setDispatcher(Dispatcher *);
    Dispatcher *dispatcher() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
