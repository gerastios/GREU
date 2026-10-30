#pragma once
#include <QItemDelegate>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) ItemDelegate : public QItemDelegate {
public:
    ItemDelegate();

    void commit();
    virtual QWidget *createEditor(QWidget *parent,
                                  QStyleOptionViewItem const &,
                                  QModelIndex const &) const override;

private:
    virtual QWidget *createWatchedEditor(QWidget *parent,
                                         QStyleOptionViewItem const &,
                                         QModelIndex const &) const
        = 0;

    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
