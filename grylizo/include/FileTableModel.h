#pragma once

#include <QAbstractTableModel>
#include "greucore/include/DetailPtr.h"
#include <initializer_list>
#include <optional>

class QColor;

namespace greu::ui {
class TableView;
}

namespace greu::tag {
class FileHolder;
}

namespace greu::grylizo {

class FileTableModel : public QAbstractTableModel {
public:
    explicit FileTableModel(ui::TableView *table);
    FileTableModel(ui::TableView *table, std::initializer_list<uint> columns);
    FileTableModel(tag::FileHolder *files, ui::TableView *table);
    FileTableModel(tag::FileHolder *files,
                   ui::TableView *table,
                   std::initializer_list<uint> columns);

    GREU_NO_MOVE(FileTableModel);
    GREU_NO_COPY(FileTableModel);

    void setFileHolder(tag::FileHolder *value, bool always = false);
    tag::FileHolder *fileHolder() const;

    void addColumn(uint typeId);

    ui::TableView *table() const;

    virtual int rowCount(QModelIndex const &) const override;
    virtual int columnCount(QModelIndex const &) const override;
    virtual QVariant data(QModelIndex const &, int role) const override;
    virtual QVariant headerData(int section, Qt::Orientation, int role) const override;
    virtual void sort(int column, Qt::SortOrder) override;

private:
    inline virtual void setFileHolderPostProc() {};
    virtual std::optional<QColor> color(QModelIndex const &);

    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
