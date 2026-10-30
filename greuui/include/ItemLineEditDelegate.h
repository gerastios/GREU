#pragma once
#include "ItemDelegate.h"

namespace greu::ui {

class GREU_PUBLIC(UI) ItemLineEditDelegate : public ItemDelegate {
public:
    ItemLineEditDelegate();

    void setValidator(QValidator *);

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
