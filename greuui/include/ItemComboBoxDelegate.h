#pragma once
#include "ItemDelegate.h"

namespace greu::core {
class Selector;
}

namespace greu::ui {

class GREU_PUBLIC(UI) ItemComboBoxDelegate : public ItemDelegate {
public:
    ItemComboBoxDelegate();

    void setValidator(QValidator *);
    void setSelector(core::Selector *);

    virtual void setEditorData(QWidget *editor, QModelIndex const &) const override;
    virtual void setModelData(QWidget *editor,
                              QAbstractItemModel *,
                              QModelIndex const &) const override;
    virtual void updateEditorGeometry(QWidget *editor,
                                      QStyleOptionViewItem const &,
                                      QModelIndex const &) const override;

private:
    virtual QWidget *createWatchedEditor(QWidget *parent,
                                         QStyleOptionViewItem const &,
                                         QModelIndex const &) const override;

    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
