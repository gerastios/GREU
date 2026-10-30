#include "DateRangeEdit.h"
#include <QHBoxLayout>
#include "DateEdit.h"
#include "greucore/include/GuardedPtr.h"
#include "utils.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct DateRangeEdit::Detail {
    EmptySig editingDone;
    GuardedPtr<QHBoxLayout> layout;
    GuardedPtr<DateEdit> from, to;
    ScopedConnection connFrom, connTo;

    void init(QDate const &f, QDate const &t, DateRangeEdit *that) {
        layout = new QHBoxLayout(that);
        addNewWidget(from, layout.get());
        addNewWidget(to, layout.get());
        that->setLayout(layout.get());
        connFrom = from->editingDone().connect([this] { editingDone(); });
        connTo = to->editingDone().connect([this] { editingDone(); });
        from->setValue(f);
        to->setValue(t);
    }
};

DateRangeEdit::DateRangeEdit(QDate const &from, QDate const &to)
    : d(GREU_MKDET()) {
    d->init(from, to, this);
}

QDate DateRangeEdit::from() const {
    return d->from->value();
}

QDate DateRangeEdit::to() const {
    return d->to->value();
}

void DateRangeEdit::setFrom(QDate const &value) {
    d->from->setValue(value);
}

void DateRangeEdit::setTo(QDate const &value) {
    d->to->setValue(value);
}

EmptySig &DateRangeEdit::editingDone() const {
    return d->editingDone;
}

} // namespace greu::ui
