#include "EditableTable.h"
#include <QAbstractTableModel>
#include <QContextMenuEvent>
#include <QImage>
#include <QMessageBox>
#include <QToolBar>
#include <QVBoxLayout>
#include "Action.h"
#include "Application.h"
#include "EditableTableModel.h"
#include "ItemDelegate.h"
#include "TableView.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/Icons.h"
#include "utils.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct EditableTable::Detail {
    struct Model : public QAbstractTableModel {
        EditableTable *table;
        EditableTableModel *tableModel = nullptr;

        Model(EditableTable *t)
            : table(t) {}

        void update() {
            layoutAboutToBeChanged();
            layoutChanged();
        }

        virtual int rowCount(QModelIndex const &) const override {
            assert(tableModel && "No table model set!");
            return tableModel->rows();
        }

        virtual int columnCount(QModelIndex const &) const override {
            assert(tableModel && "No table model set!");
            return tableModel->columns();
        }

        virtual QVariant data(QModelIndex const &index, int role) const override {
            assert(tableModel && "No table model set!");
            if (!index.isValid())
                return QVariant();
            if (role == Qt::DecorationRole) {
                auto img = tableModel->image(index.row(), index.column());
                if (img && !img->isNull()) {
                    if (table->table()->rowHeight(index.row()) != img->height())
                        table->table()->setRowHeight(index.row(), img->height());
                    if (table->table()->columnWidth(index.column()) != img->width())
                        table->table()->setColumnWidth(index.column(), img->width());
                    return *img;
                }
            }
            if (role != Qt::DisplayRole && role != Qt::EditRole)
                return QVariant();
            return tableModel->value(index.row(), index.column());
        }

        virtual QVariant headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const override {
            assert(tableModel && "No table model set!");
            if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
                return tableModel->columnTitle(section);
            return QVariant();
        }

        virtual Qt::ItemFlags flags(QModelIndex const &index) const override {
            if (index.isValid())
                return QAbstractTableModel::flags(index) | Qt::ItemIsEditable;
            else
                return Qt::ItemIsEnabled;
        }

        virtual bool setData(QModelIndex const &index, QVariant const &value, int role) override {
            assert(tableModel && "No table model set!");
            if (index.isValid() && role == Qt::EditRole)
                tableModel->setValue(index.row(), index.column(), value);
            return true;
        }

        void beginInsert(int row) { beginInsertRows(QModelIndex(), row, row); }

        void endInsert() { endInsertRows(); }

        void beginRemove(int row) { beginRemoveRows(QModelIndex(), row, row); }

        void endRemove() { endRemoveRows(); }

        void setModel(EditableTableModel *const value) { tableModel = value; }
    };

    struct EditTable : public TableView {
        EmptySig lostFocus;
        EditTable(QString const &name)
            : TableView(name) {
            setFocusPolicy(Qt::WheelFocus);
        }
        virtual void focusOutEvent(QFocusEvent *) override { lostFocus(); }
    };

    EditableTable *that;
    QString const name;
    GuardedPtr<EditTable> table;
    ItemDelegate *itemDelegate;
    GuardedPtr<Model> model;
    GuardedPtr<QVBoxLayout> layout;
    GuardedPtr<QToolBar> toolbar;

    GuardedPtr<Action> actAddRow, actDeleteRows, actInsertRow, actSelectAll, actDeselectAll,
        actDeleteRowCtx, actInsertRowCtx;
    vector<Action *> actions;

    EmptySig editingDone;
    ScopedConnection finishConn;
    bool enabled = false;

    Detail(QString const &name)
        : name(name) {}

    void setupUi(ItemDelegate *id, EditableTable *that) {
        that->setItemDelegate(id);
        assert(itemDelegate && "No ItemDelegate set");
        actAddRow = new Action(GREU_ICON(add), QObject::tr("Add row"));
        actDeleteRows = new Action(GREU_ICON(subtract), QObject::tr("Delete rows"));
        actInsertRow = new Action(GREU_ICON(import), QObject::tr("Insert row"));
        actSelectAll = new Action(GREU_ICON(select_all), QObject::tr("Select all"));
        actDeselectAll = new Action(GREU_ICON(select_none), QObject::tr("Select none"));
        actDeleteRowCtx = new Action(GREU_ICON(subtract), QObject::tr("Delete row"));
        actInsertRowCtx = new Action(GREU_ICON(import), QObject::tr("Insert row"));
        finishConn = table->lostFocus.connect([this] { editingDone(); });
        layout = new QVBoxLayout;
        addNewWidget(toolbar, layout.get());
        that->setLayout(layout.get());
        table->setEditTriggers(QAbstractItemView::AllEditTriggers);
        table->setModel(model.get());
        layout->addWidget(table.get());
        table->setItemDelegate(itemDelegate);
        setupActions(toolbar.get());
    }

    void setEnabled(bool value) {
        enabled = value;
        update();
    }

    void update() {
        enableActions();
        model->update();
    }

    void setupActions(QToolBar *toolbar) {
        toolbar->addAction(actAddRow.get());
        ui::connect(actAddRow.get(), [this] {
            endEditing();
            model->beginInsert(model->tableModel->rows());
            if (model->tableModel->addRow()) {
                model->endInsert();
                table->setCurrentIndex(model->index(model->tableModel->rows() - 1, 0));
            }
        });
        toolbar->addAction(actInsertRow.get());
        ui::connect(actInsertRow.get(), [this] {
            auto r = selectedRow();
            if (r < 0)
                return;
            endEditing();
            model->beginInsert(r);
            if (model->tableModel->insertRow(r)) {
                model->endInsert();
                table->setCurrentIndex(model->index(r, 0));
            }
        });
        toolbar->addAction(actDeleteRows.get());
        ui::connect(actDeleteRows.get(), [this] {
            auto r = selectedRows();
            if (r.size() == 0)
                return;
            endEditing();
            std::sort(r.begin(), r.end(), std::greater<int>());
            for (auto i : r) {
                model->beginRemove(i);
                if (model->tableModel->deleteRow(i))
                    model->endRemove();
            }
        });
        toolbar->addSeparator();
        toolbar->addAction(actSelectAll.get());
        ui::connect(actSelectAll.get(), [this] { table->selectAll(); });
        toolbar->addAction(actDeselectAll.get());
        ui::connect(actDeselectAll.get(), [this] { table->clearSelection(); });
        toolbar->addSeparator();
        for (auto a : actions)
            toolbar->addAction(a);

        table->addContextAction(actInsertRowCtx.get(), [this](int row, int, QAbstractTableModel *) {
            if (row < 0)
                return;
            endEditing();
            model->beginInsert(row);
            if (model->tableModel->insertRow(row)) {
                model->endInsert();
                table->setCurrentIndex(model->index(row, 0));
            }
        });
        table->addContextAction(actDeleteRowCtx.get(), [this](int row, int, QAbstractTableModel *) {
            if (row < 0)
                return;
            endEditing();
            model->beginRemove(row);
            if (model->tableModel->deleteRow(row))
                model->endRemove();
        });
    }

    void endEditing() {
        auto w = Application::focusWidget();
        model->tableModel->closeEditor(w);
    }

    int selectedRow() const {
        auto s = table->selectionModel()->selectedRows();
        if (s.size() != 1) {
            QMessageBox::critical(table.get(),
                                  QObject::tr("Error"),
                                  QObject::tr("Please select one line!"));
            return -1;
        }
        return s.at(0).row();
    }

    vector<int> selectedRows() const {
        auto s = table->selectionModel()->selectedRows();
        if (s.size() == 0) {
            QMessageBox::critical(table.get(),
                                  QObject::tr("Error"),
                                  QObject::tr("No line selected!"));
            return std::vector<int>();
        }
        std::vector<int> res;
        for (auto i : s)
            res.push_back(i.row());
        return res;
    }

    void enableActions() {
        actAddRow->setEnabled(enabled);
        actInsertRow->setEnabled(enabled);
        actDeleteRows->setEnabled(enabled);
        actSelectAll->setEnabled(enabled);
        actDeselectAll->setEnabled(enabled);
        for (auto a : actions)
            a->setEnabled(enabled);
    }
};

EditableTable::EditableTable(QString const &name)
    : d(GREU_MKDET(name)) {
    d->that = this;
    d->table = new Detail::EditTable(name);
    d->model = new Detail::Model(this);
}

void EditableTable::setModel(EditableTableModel *v) {
    d->model->setModel(v);
}

void EditableTable::setupUi(ItemDelegate *id) {
    d->setupUi(id, this);
}

void EditableTable::update() {
    d->update();
}

TableView *EditableTable::table() const {
    return d->table.get();
}

void EditableTable::addAction(Action *a) {
    d->actions.push_back(a);
}

bool EditableTable::isEnabled() const {
    return d->enabled;
}

void EditableTable::setEnabled(bool value) {
    d->setEnabled(value);
}

ItemDelegate *EditableTable::itemDelegate() const {
    return d->itemDelegate;
}

void EditableTable::setItemDelegate(ItemDelegate *v) {
    d->itemDelegate = v;
}

QAbstractTableModel *EditableTable::tableModel() const {
    return d->model.get();
}

EmptySig &EditableTable::editingDone() const {
    return d->editingDone;
}

} // namespace greu::ui
