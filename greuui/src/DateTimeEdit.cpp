#include "DateTimeEdit.h"
#include <QDateTimeEdit>
#include <QLineEdit>
#include "Action.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/Icons.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct DateTimeEdit::Detail {
    bool cleared = true;
    EmptySig editingDone;
    GuardedPtr<Action> actClear;

    void init(QDateTime const &value, DateTimeEdit *that) {
        actClear = new Action(GREU_ICON(erase));
        that->setCalendarPopup(true);
        that->lineEdit()->addAction(actClear.get(), QLineEdit::LeadingPosition);
        ui::connect(actClear.get(), [this, that] {
            cleared = true;
            that->lineEdit()->setText("");
            editingDone();
        });
        ui::connectTextEdited(that->lineEdit(), [this](QString const &) {
            if (cleared) {
                cleared = false;
                editingDone();
            }
        });
        ui::connectDateTimeChanged(that, [this](auto dt) {
            if (cleared && dt.isValid()) {
                cleared = false;
                editingDone();
            }
        });
        actClear->setToolTip(QObject::tr("Clear"));
        setValue(value, that);
        that->lineEdit()->setText("");
    }

    QDateTime value(DateTimeEdit const *const that) const {
        if (cleared)
            return QDateTime();
        return that->dateTime();
    }

    void setValue(QDateTime const &value, DateTimeEdit *const that) {
        if (!value.isValid()) {
            cleared = true;
            that->lineEdit()->deselect();
            that->lineEdit()->clear();
        } else {
            cleared = false;
            that->setDateTime(value);
        }
    }
};

DateTimeEdit::DateTimeEdit(QDateTime const &value)
    : d(GREU_MKDET()) {
    d->init(value, this);
}

QDateTime DateTimeEdit::value() const {
    return d->value(this);
}

void DateTimeEdit::setValue(QDateTime const &value) {
    d->setValue(value, this);
}

EmptySig &DateTimeEdit::editingDone() const {
    return d->editingDone;
}

QLineEdit *DateTimeEdit::editor() {
    return lineEdit();
}

void DateTimeEdit::paintEvent(QPaintEvent *event) {
    QDateTimeEdit::paintEvent(event);
    if (d->cleared)
        lineEdit()->setText("");
}

} // namespace greu::ui
