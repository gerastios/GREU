#include "ExpandingComboBox.h"
#include <QEvent>
#include <QStyleOptionFrame>
#include "LineEdit.h"
#include "connect.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct ExpandingComboBox::Detail {
    bool widgetOwnsGeometry = false;
    int originalWidth = -1;
    QMetaObject::Connection conn;

    void changeEvent(QEvent *event, ExpandingComboBox *that) {
        switch (event->type()) {
        case QEvent::FontChange:
        case QEvent::StyleChange:
        case QEvent::ContentsRectChange:
            updateMinimumWidth(that);
            break;
        default:
            break;
        }
        that->QComboBox::changeEvent(event);
    }

    void updateMinimumWidth(ExpandingComboBox *that) {
        auto lineEdit = that->lineEdit();
        if (lineEdit) {
            QMargins const tm = lineEdit->textMargins();
            QMargins const cm = lineEdit->contentsMargins();
            int const width = tm.left() + tm.right() + cm.left() + cm.right()
                              + 4 /*horizontalMargin in qlineedit.cpp*/;
            QStyleOptionComboBox opt;
            that->initStyleOption(&opt);
            int minWidth = that->style()
                               ->sizeFromContents(QStyle::CT_LineEdit,
                                                  &opt,
                                                  QSize(width, 0).expandedTo(QSize{0, 0}),
                                                  that)
                               .width();
            that->setMinimumWidth(minWidth);
        }
    }
};

ExpandingComboBox::ExpandingComboBox(QWidget *parent)
    : ComboBox(parent)
    , d(GREU_MKDET()) {
    d->conn = ui::connectEditTextChanged(this, [this](auto) {
        int oldWidth = width();
        if (d->originalWidth == -1)
            d->originalWidth = oldWidth;
        if (QWidget *parent = parentWidget()) {
            QPoint position = pos();
            auto lineEdit = this->lineEdit();
            int hintWidth = minimumWidth()
                            + fontMetrics().horizontalAdvance(lineEdit->displayText());
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

void ExpandingComboBox::setWidgetOwnsGeometry(bool v) {
    d->widgetOwnsGeometry = v;
}

bool ExpandingComboBox::widgetOwnsGeometry() const {
    return d->widgetOwnsGeometry;
}

void ExpandingComboBox::changeEvent(QEvent *event) {
    d->changeEvent(event, this);
}

} // namespace greu::ui
