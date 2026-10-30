#pragma once
#include <QWidget>
#include "EditAlerter.h"
#include "greucore/include/DetailPtr.h"

class QAbstractTableModel;

namespace greu::ui {

class Action;

class EditableTableModel;
class ItemDelegate;
class TableView;

class GREU_PUBLIC(UI) EditableTable : public QWidget, public EditAlerter {
public:
    explicit EditableTable(QString const &name);

    void setModel(EditableTableModel *);

    void setupUi(ItemDelegate *);
    void update();

    TableView *table() const;

    void addAction(Action *);

    bool isEnabled() const;
    void setEnabled(bool);

    ItemDelegate *itemDelegate() const;
    void setItemDelegate(ItemDelegate *);

    QAbstractTableModel *tableModel() const;

    virtual core::EmptySig &editingDone() const override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

} // namespace greu::ui
