#include "BusyCursor.h"
#include <QGuiApplication>
#include <QWidget>

namespace greu::ui {

using namespace core;

struct BusyCursor::Detail {
    QCursor cursor;
};

BusyCursor::BusyCursor()
    : d(GREU_MKDET()) {
    QGuiApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
}

BusyCursor::~BusyCursor() {
    QGuiApplication::restoreOverrideCursor();
}

} // namespace greu::ui
