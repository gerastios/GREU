#include "Notepad.h"
#include <QTextEdit>
#include <QVBoxLayout>
#include "greucore/include/GuardedPtr.h"
#include "utils.h"

namespace greu::ui {

using namespace core;

struct Notepad::Detail {
    GuardedPtr<QTextEdit> edit;
    GuardedPtr<QVBoxLayout> layout;

    void setupUi() {
        layout = new QVBoxLayout;
        addNewWidget(edit, layout.get());
    }
};

Notepad::Notepad(QString const &name)
    : Dialog(name)
    , d(GREU_MKDET()) {}

void Notepad::setupUi() {
    d->setupUi();
    setLayout(d->layout.get());
}

QString Notepad::text() const {
    return d->edit->toPlainText();
}

void Notepad::setText(QString const &value) {
    d->edit->setText(value);
}

} // namespace greu::ui
