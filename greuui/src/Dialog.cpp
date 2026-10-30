#include "Dialog.h"
#include <QEvent>
#include "Settings.h"

namespace greu::ui {

using namespace core;

struct Dialog::Detail {
    bool restored = false;
};

Dialog::Dialog(Qt::WindowFlags f)
    : QDialog(nullptr, f)
    , d(GREU_MKDET()) {}

Dialog::Dialog(QString const &name, Qt::WindowFlags f)
    : QDialog(nullptr, f)
    , d(GREU_MKDET()) {
    setObjectName(name);
}

bool Dialog::event(QEvent *const event) {
    if ((!objectName().isEmpty()) && event->type() == QEvent::Paint && !d->restored) {
        Settings::instance().restore(this);
        d->restored = true;
    }
    return QDialog::event(event);
}

void Dialog::closeEvent(QCloseEvent *event) {
    if (!objectName().isEmpty()) {
        Settings::instance().save(this);
    }
    QDialog::closeEvent(event);
}

} // namespace greu::ui
