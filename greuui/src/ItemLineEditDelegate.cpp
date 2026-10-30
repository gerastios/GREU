#include "ItemLineEditDelegate.h"
#include "ExpandingLineEdit.h"

namespace greu::ui {

using namespace core;

struct ItemLineEditDelegate::Detail {
    QValidator *validator = nullptr;

    QWidget *createEditor(QWidget *parent) {
        auto res = new ExpandingLineEdit(parent);
        res->setFrame(res->style()->styleHint(QStyle::SH_ItemView_DrawDelegateFrame, 0, res));
        if (!res->style()->styleHint(QStyle::SH_ItemView_ShowDecorationSelected, 0, res))
            res->setWidgetOwnsGeometry(true);
        if (validator)
            res->setValidator(validator);
        return res;
    }

    void setEditorData(QWidget *editor, QModelIndex const &index) const {
        QString value = index.model()->data(index, Qt::EditRole).toString();
        auto ed = dynamic_cast<QLineEdit *>(editor);
        if (ed)
            ed->setText(value);
    }

    void setModelData(QWidget *editor, QAbstractItemModel *model, QModelIndex const &index) const {
        auto ed = dynamic_cast<QLineEdit *>(editor);
        if (ed)
            model->setData(index, ed->text());
    }
};

ItemLineEditDelegate::ItemLineEditDelegate()
    : d(GREU_MKDET()) {}

void ItemLineEditDelegate::setValidator(QValidator *value) {
    d->validator = value;
}

void ItemLineEditDelegate::setEditorData(QWidget *editor, QModelIndex const &index) const {
    d->setEditorData(editor, index);
}

void ItemLineEditDelegate::setModelData(QWidget *editor,
                                        QAbstractItemModel *model,
                                        QModelIndex const &index) const {
    d->setModelData(editor, model, index);
}

void ItemLineEditDelegate::updateEditorGeometry(QWidget *editor,
                                                QStyleOptionViewItem const &option,
                                                QModelIndex const &) const {
    editor->setGeometry(option.rect);
}

QWidget *ItemLineEditDelegate::createWatchedEditor(QWidget *parent,
                                                   QStyleOptionViewItem const &,
                                                   QModelIndex const &) const {
    return d->createEditor(parent);
}

} // namespace greu::ui
