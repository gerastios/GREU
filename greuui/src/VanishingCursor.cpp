#include "VanishingCursor.h"
#include <QWidget>

namespace greu::ui {

using namespace core;

struct VanishingCursor::Detail {
    QWidget *widget;
    QCursor cursor;
};

VanishingCursor::VanishingCursor(QWidget *w)
    : d(GREU_MKDET()) {
    d->widget = w;
    d->cursor = w->cursor();
}

VanishingCursor::~VanishingCursor() {
    d->widget->setCursor(d->cursor);
}

void VanishingCursor::show() {
    d->widget->setCursor(d->cursor);
}

void VanishingCursor::hide() {
    d->widget->setCursor(Qt::BlankCursor);
}

} // namespace greu::ui
