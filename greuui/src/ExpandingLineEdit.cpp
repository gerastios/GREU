#include "ExpandingLineEdit.h"
#include <QEvent>
#include <QStyleOptionFrame>
#include "connect.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct ExpandingLineEdit::Detail {
    bool widgetOwnsGeometry = false;
    int originalWidth = -1;
    QMetaObject::Connection conn;

    void changeEvent(QEvent *event, ExpandingLineEdit *that) {
        switch (event->type()) {
        case QEvent::FontChange:
        case QEvent::StyleChange:
        case QEvent::ContentsRectChange:
            updateMinimumWidth(that);
            break;
        default:
            break;
        }
        that->QLineEdit::changeEvent(event);
    }

    void updateMinimumWidth(ExpandingLineEdit *that) {
        QMargins const tm = that->textMargins();
        QMargins const cm = that->contentsMargins();
        int const width = tm.left() + tm.right() + cm.left() + cm.right()
                          + 4 /*horizontalMargin in qlineedit.cpp*/;
        QStyleOptionFrame opt;
        that->initStyleOption(&opt);
        int min_width = that->style()
                            ->sizeFromContents(QStyle::CT_LineEdit,
                                               &opt,
                                               QSize(width, 0).expandedTo(QSize{0, 0}),
                                               that)
                            .width();
        that->setMinimumWidth(min_width);
    }

    ~Detail() { QObject::disconnect(conn); }
};

ExpandingLineEdit::ExpandingLineEdit(QWidget *parent)
    : QLineEdit(parent)
    , d(GREU_MKDET()) {
    d->conn = ui::connectTextChanged(this, [this](QString const &) {
        int oldWidth = width();
        if (d->originalWidth == -1)
            d->originalWidth = oldWidth;
        if (QWidget *parent = parentWidget()) {
            QPoint position = pos();
            int hintWidth = minimumWidth() + fontMetrics().horizontalAdvance(displayText());
            int parentWidth = parent->width();
            int maxWidth = isRightToLeft() ? position.x() + oldWidth : parentWidth - position.x();
            int newWidth = qBound(d->originalWidth, hintWidth, maxWidth);
            if (d->widgetOwnsGeometry)
                setMaximumWidth(newWidth);
            if (isRightToLeft())
                move(position.x() - newWidth + oldWidth, position.y());
            resize(newWidth, height());
        }
    });
    d->updateMinimumWidth(this);
}

void ExpandingLineEdit::setWidgetOwnsGeometry(bool v) {
    d->widgetOwnsGeometry = v;
}

bool ExpandingLineEdit::widgetOwnsGeometry() const {
    return d->widgetOwnsGeometry;
}

void ExpandingLineEdit::changeEvent(QEvent *event) {
    d->changeEvent(event, this);
}

} // namespace greu::ui
