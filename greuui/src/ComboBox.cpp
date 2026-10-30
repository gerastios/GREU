#include "ComboBox.h"
#include <QListView>
#include <QMenu>
#include <QPaintEvent>
#include <QPainter>
#include "Action.h"
#include "LineEdit.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/Selector.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct ComboBox::Detail {
    struct ComboBoxListView : public QListView {
        GuardedPtr<ComboBox> comboBox;
        GuardedPtr<QMenu> menu;
        GuardedPtr<Action> actRemove;
        QPoint ctxPos;

    public:
        ComboBoxListView(ComboBox *comboBox)
            : comboBox(comboBox) {
            menu = new QMenu;
            actRemove = new Action(QObject::tr("&Remove item"));
            menu->addAction(actRemove.get());
            ui::connect(actRemove.get(), [this] {
                auto seltr = this->comboBox->selector();
                if (!seltr || !seltr->isDeletionAllowed())
                    return;
                auto row = this->indexAt(ctxPos).row();
                seltr->remove((seltr->*seltr->atFunction())(row).first);
                this->comboBox->hidePopup();
            });
        }

        void resizeEvent(QResizeEvent *event) override {
            resizeContents(viewport()->width(), contentsSize().height());
            QListView::resizeEvent(event);
        }

        void initViewItemOption(QStyleOptionViewItem *option) const override {
            QListView::initViewItemOption(option);
            option->showDecorationSelected = true;
            option->font = comboBox->font();
        }

        void paintEvent(QPaintEvent *e) override {
            QStyleOptionComboBox opt;
            opt.initFrom(comboBox.get());
            opt.editable = comboBox->isEditable();
            if (comboBox->style()->styleHint(QStyle::SH_ComboBox_Popup, &opt, comboBox.get())) {
                //we paint the empty menu area to avoid having blank space that can happen when scrolling
                QStyleOptionMenuItem menuOpt;
                menuOpt.initFrom(this);
                menuOpt.palette = palette();
                menuOpt.state = QStyle::State_None;
                menuOpt.checkType = QStyleOptionMenuItem::NotCheckable;
                menuOpt.menuRect = e->rect();
                menuOpt.maxIconWidth = 0;
                menuOpt.reservedShortcutWidth = 0;
                QPainter p(viewport());
                comboBox->style()->drawControl(QStyle::CE_MenuEmptyArea, &menuOpt, &p, this);
            }
            QListView::paintEvent(e);
        }

        virtual void contextMenuEvent(QContextMenuEvent *event) override {
            if (!comboBox->selector() || !comboBox->selector()->isDeletionAllowed()) {
                event->ignore();
                return;
            }
            ctxPos = event->pos();
            event->accept();
            menu->popup(QCursor::pos());
        }
    };

    Selector *selector = nullptr;
    EmptySig editingDone;

    GuardedPtr<ComboBoxListView> listView;
    GuardedPtr<LineEdit> lineEdit;
    ScopedConnection scopedConnLineEdit;
    QMetaObject::Connection connLineEdit;
    ScopedConnection connSelector;

    void init(ComboBox *const that) {
        listView = new ComboBoxListView(that);
        lineEdit = new LineEdit;
        lineEdit->setVisible(false);
        that->setView(listView.get());
        that->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    }

    void setSelector(Selector *const selector, ComboBox *const that) {
        this->selector = selector;
        that->clear();
        that->setCurrentText("");
        if (connLineEdit)
            QObject::disconnect(connLineEdit);
        if (connSelector.connected())
            connSelector.disconnect();
        if (scopedConnLineEdit.connected())
            scopedConnLineEdit.disconnect();
        if (selector) {
            setItems(that);
            if (selector->isUserValuesAllowed()) {
                that->setEditable(true);
                that->setLineEdit(lineEdit.get());
                lineEdit->setVisible(true);
                scopedConnLineEdit = lineEdit->editingDone().connect([this] { editingDone(); });
            } else {
                that->setEditable(false);
                ui::connectIndex(that, [this](int) { editingDone(); });
            }
            connSelector = selector->valuesChanged().connect([this, that] { setItems(that); });
        } else {
            that->setEditable(false);
        }
    }

    void setItems(ComboBox *const that) {
        auto text = that->currentText();
        that->clear();
        auto at = selector->atFunction();
        for (size_t i = 0; i < selector->size(); i++) {
            auto v = (selector->*at)(i);
            that->addItem(v.second, v.first);
        }
        that->setCurrentText(text);
    }

    ~Detail() {
        if (connLineEdit)
            QObject::disconnect(connLineEdit);
    }
};

ComboBox::ComboBox(QWidget *parent)
    : QComboBox(parent)
    , d(GREU_MKDET()) {
    d->init(this);
    ui::connectFinished(this->lineEdit(), [this] { d->editingDone(); });
    ui::connectTextChanged(this, [this] { editingDone(); });
}

Selector *ComboBox::selector() const {
    return d->selector;
}

void ComboBox::setSelector(Selector *const v) {
    d->setSelector(v, this);
}

int ComboBox::id() const {
    return currentData().toInt();
}

LineEdit *ComboBox::lineEdit() const {
    return d->lineEdit.get();
}

EmptySig &ComboBox::editingDone() const {
    return d->editingDone;
}

} // namespace greu::ui
