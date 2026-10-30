#include "ItemComboBoxDelegate.h"
#include "ExpandingComboBox.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct ItemComboBoxDelegate::Detail {
    QValidator *validator = nullptr;
    core::Selector *selector = nullptr;

    auto createEditor(QWidget *parent) {
        auto res = new ExpandingComboBox(parent);
        res->setFrame(
            res->QComboBox::style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, res));
        res->setValidator(validator);
        res->setSelector(selector);
        return res;
    }

    void setEditorData(QWidget *editor, QModelIndex const &index) const {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        auto ed = dynamic_cast<QComboBox *>(editor);
        if (ed)
            ed->setCurrentText(value);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, QModelIndex const &index) const {
        auto ed = dynamic_cast<QComboBox *>(editor);
        if (ed)
            model->setData(index, ed->currentText());
    }
};

ItemComboBoxDelegate::ItemComboBoxDelegate()
    : d(GREU_MKDET()) {}

void ItemComboBoxDelegate::setValidator(QValidator *v) {
    d->validator = v;
}

void ItemComboBoxDelegate::setSelector(Selector *v) {
    d->selector = v;
}

void ItemComboBoxDelegate::setEditorData(QWidget *editor, QModelIndex const &index) const {
    d->setEditorData(editor, index);
}

void ItemComboBoxDelegate::setModelData(QWidget *editor,
                                        QAbstractItemModel *model,
                                        QModelIndex const &index) const {
    d->setModelData(editor, model, index);
}

void ItemComboBoxDelegate::updateEditorGeometry(QWidget *editor,
                                                QStyleOptionViewItem const &option,
                                                QModelIndex const &) const {
    editor->setGeometry(option.rect);
}

QWidget *ItemComboBoxDelegate::createWatchedEditor(QWidget *parent,
                                                   QStyleOptionViewItem const &,
                                                   QModelIndex const &) const {
    return d->createEditor(parent);
}

} // namespace greu::ui
