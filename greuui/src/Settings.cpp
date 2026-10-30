#include "Settings.h"
#include <QDialog>
#include <QHeaderView>
#include <QLayout>
#include <QMainWindow>
#include <QSplitter>
#include <QTabWidget>
#include <QWidget>
#include "Accordion.h"
#include <functional>
#include "Application.h"

using namespace std;

namespace greu::ui {

namespace {
QString geometry{"geometry"}, state{"state"}, activeTab{"active"}, fullScreen{"fullscreen"};

void processChildren(QObject const *o, function<void(QWidget *)> f) {
    for (auto c : o->children()) {
        auto w = dynamic_cast<QWidget *>(c);
        if (w) {
            f(w);
        } else {
            auto l = dynamic_cast<QLayout *>(c);
            if (l)
                processChildren(l, f);
        }
    }
}

} // namespace

Settings::Settings()
    : core::Settings(Application::instance()->applicationName()) {}

Settings &Settings::instance() {
    static Settings i;
    return i;
}


void Settings::save(QWidget const *widget) {
    if (widget->objectName().length() > 0) {
        auto w = dynamic_cast<QMainWindow const *>(widget);
        auto s = dynamic_cast<QSplitter const *>(widget);
        auto t = dynamic_cast<QTabWidget const *>(widget);
        auto h = dynamic_cast<QHeaderView const *>(widget);
        auto a = dynamic_cast<Accordion const *>(widget);
        auto d = dynamic_cast<QDialog const *>(widget);
        auto n = widget->objectName();
        if (w) {
            setValue(state + "/" + n, w->saveState());
            setValue(geometry + "/" + n, w->saveGeometry());
            setValue(fullScreen + "/" + n, w->isFullScreen());
        }
        if (d)
            setValue(geometry + "/" + n, d->saveGeometry());
        if (s)
            setValue(state + "/" + n, s->saveState());
        if (h)
            setValue(state + "/" + n, h->saveState());
        if (t)
            setValue(activeTab + "/" + n, t->currentIndex());
        if (a)
            setValue(activeTab + "/" + n, a->currentIndex());
    }
    processChildren(widget, [this](auto o) { save(o); });
}

void Settings::restore(QWidget *widget) {
    if (widget->objectName().length() > 0) {
        auto w = dynamic_cast<QMainWindow *>(widget);
        auto s = dynamic_cast<QSplitter *>(widget);
        auto t = dynamic_cast<QTabWidget *>(widget);
        auto h = dynamic_cast<QHeaderView *>(widget);
        auto a = dynamic_cast<Accordion *>(widget);
        auto d = dynamic_cast<QDialog *>(widget);
        auto n = widget->objectName();
        if (w) {
            w->restoreState(value(state + "/" + n).toByteArray());
            w->restoreGeometry(value(geometry + "/" + n).toByteArray());
            if (value(fullScreen + "/" + n).toBool())
                w->showFullScreen();
        }
        if (d)
            d->restoreGeometry(value(geometry + "/" + n).toByteArray());
        if (s)
            s->restoreState(value(state + "/" + n).toByteArray());
        if (t)
            t->setCurrentIndex(value(activeTab + "/" + n).toInt());
        if (h)
            h->restoreState(value(state + "/" + n).toByteArray());
        if (a)
            a->setCurrentIndex(value(activeTab + "/" + n).toInt());
    }
    processChildren(widget, [this](auto o) { restore(o); });
}

} // namespace greu::ui
