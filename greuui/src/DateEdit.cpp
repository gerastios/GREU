#include "DateEdit.h"
#include <QDateTimeEdit>
#include <QLineEdit>
#include "Action.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/Icons.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct DateEdit::Detail {
    bool cleared = true;
    EmptySig editingDone;
    GuardedPtr<Action> actClear;

    void init(DateEdit *that) {
        actClear = new Action(GREU_ICON(erase));
        that->addAction(actClear.get());
        that->setCalendarPopup(true);
        that->lineEdit()->addAction(actClear.get(), QLineEdit::LeadingPosition);
        ui::connectTextEdited(that->lineEdit(), [this](QString const &) {
            if (cleared) {
                cleared = false;
                editingDone();
            }
        });
        ui::connectDateChanged(that, [this](QDate const &) {
            cleared = false;
            editingDone();
        });
        ui::connect(actClear.get(), [this, that] {
            cleared = true;
            that->lineEdit()->setText("");
            editingDone();
        });
        actClear->setToolTip(QObject::tr("Clear"));
        that->lineEdit()->setText("");
    }

    void setValue(QDate const &value, DateEdit *that) {
        if (value.isNull()) {
            cleared = true;
            that->lineEdit()->setText("");
        } else {
            cleared = false;
            that->setDate(value);
        }
    }
};

DateEdit::DateEdit(QDate const &value)
    : d(GREU_MKDET()) {
    d->init(this);
    d->setValue(value, this);
}

QDate DateEdit::value() const {
    if (d->cleared)
        return QDate();
    return date();
}

void DateEdit::setValue(QDate const &value) {
    d->setValue(value, this);
}

EmptySig &DateEdit::editingDone() const {
    return d->editingDone;
}

void DateEdit::paintEvent(QPaintEvent *event) {
    QDateEdit::paintEvent(event);
    if (d->cleared) {
        lineEdit()->setText("");
    }
}

} // namespace greu::ui
