#include "FileTableModel.h"
#include <QBuffer>
#include "greucore/include/vectorUtils.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/AttributeTypes.h"
#include "greutag/include/File.h"
#include "greutag/include/FileContainer.h"
#include "greutag/include/Tag.h"
#include "greuui/include/TableView.h"
#include <vector>

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct FileTableModel::Detail {
    FileHolder *files;
    TableView *table;
    vector<AttributeType *> columns;
    FileTableModel *model;

    Qt::SortOrder order;
    int sortColumn;
    bool isSorted = false;

    Detail(FileHolder *files, TableView *table, initializer_list<uint> columns = {})
        : files(files)
        , table(table) {
        for (auto &c : columns)
            addColumn(c);
    }

    void setFileHolder(FileHolder *value, FileTableModel *that, bool always) {
        if (!always && files == value)
            return;
        that->layoutAboutToBeChanged();
        files = value;
        if (!table->isSortingEnabled())
            table->setSortingEnabled(true);
        if (isSorted) {
            sort(sortColumn, order, that);
        }
        that->setFileHolderPostProc();
        that->layoutChanged();
    }

    FileHolder *fileHolder() const { return files; }

    void addColumn(uint typeId) {
        auto type = tag::AttributeTypes::type(typeId);
        if (core::contains(columns, type))
            return;
        columns.push_back(type);
    }

    int rowCount() const {
        if (!files)
            return 0;
        return files->size();
    }

    int columnCount() const { return columns.size(); }

    QVariant data(QModelIndex const &index, int role) const {
        if (!files)
            return QVariant();
        auto sz = table->rowHeight(index.row());
        if (files->size() == 0)
            return QVariant();
        if (!index.isValid())
            return QVariant();
        if ((size_t) index.row() > files->size())
            return QVariant();
        auto tag = files->at(index.row())->tag();
        if (role == Qt::ItemDataRole::ForegroundRole && tag->isModified()) {
            return QBrush(Qt::blue);
        }
        tag::AttributeType *attType;
        try {
            attType = columns[index.column()];
        } catch (...) {
            return QVariant();
        }
        if (attType->id() == tag::AttributeTypes::images()) {
            auto imgatt = tag->attributeImages();
            if (imgatt) {
                auto img = imgatt->mainValue();
                if (img.isNull()) {
                    if (role == Qt::DecorationRole)
                        return QImage();
                    else
                        return QVariant();
                }
                if (role == Qt::DecorationRole) {
                    return img.scaled(sz, sz, Qt::AspectRatioMode::KeepAspectRatio);
                } else if (role == Qt::ItemDataRole::ToolTipRole) {
                    QByteArray ba;
                    QBuffer bu(&ba);
                    bu.open(QIODevice::WriteOnly);
                    img.save(&bu, "PNG");
                    return QString("<img src='data:image/jpg;base64, %0'><br>%1 - %2")
                        .arg(QString(ba.toBase64()), tag->songArtist(), tag->songTitle());
                }
            }
        }
        if (role == Qt::ItemDataRole::BackgroundRole) {
            auto colorc = model->color(index);
            if (colorc)
                return colorc.value();
        }
        if (role != Qt::DisplayRole)
            return QVariant();
        auto att = tag->attribute(attType->id());
        if (!att)
            return QVariant();
        auto sa = dynamic_cast<tag::AttributeTypes::PlainStringAttribute *>(att);
        if (sa)
            return sa->value();
        auto ta = dynamic_cast<tag::AttributeTypes::PlainTimeAttribute *>(att);
        if (ta)
            return ta->value();
        auto ia = dynamic_cast<tag::AttributeTypes::PlainUintAttribute *>(att);
        if (ia)
            return ia->value();
        return QVariant();
    }

    QVariant headerData(int section, Qt::Orientation orientation, int role) const {
        if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
            return QVariant();
        tag::AttributeType *attType;
        try {
            attType = columns[section];
        } catch (...) {
            return QVariant();
        }
        return attType->longUpper();
    }

    void sort(int column, Qt::SortOrder order, FileTableModel *that) {
        auto container = dynamic_cast<tag::FileContainer *>(files);
        if (!container)
            return;
        if (!files || files->size() == 0)
            return;
        tag::AttributeType *attType;
        try {
            attType = columns[column];
        } catch (...) {
            return;
        }
        auto typeId = attType->id();
        if (typeId == tag::AttributeTypes::null())
            return;
        tag::sort(container->begin(), container->end(), typeId, order);
        isSorted = true;
        sortColumn = column;
        this->order = order;
        that->layoutChanged();
    }
};

FileTableModel::FileTableModel(TableView *table)
    : QAbstractTableModel(table)
    , d(GREU_MKDET(nullptr, table)) {
    d->model = this;
}

FileTableModel::FileTableModel(TableView *table, initializer_list<uint> columns)
    : QAbstractTableModel(table)
    , d(GREU_MKDET(nullptr, table, columns)) {
    d->model = this;
}

FileTableModel::FileTableModel(FileHolder *files, TableView *table)
    : QAbstractTableModel(table)
    , d(GREU_MKDET(files, table)) {
    d->model = this;
}

FileTableModel::FileTableModel(FileHolder *files, TableView *table, initializer_list<uint> columns)
    : QAbstractTableModel(table)
    , d(GREU_MKDET(files, table, columns)) {
    d->model = this;
}

void FileTableModel::setFileHolder(FileHolder *v, bool always) {
    d->setFileHolder(v, this, always);
}

FileHolder *FileTableModel::fileHolder() const {
    return d->fileHolder();
}

optional<QColor> FileTableModel::color(QModelIndex const &) {
    return std::nullopt;
}

void FileTableModel::addColumn(uint typeId) {
    d->addColumn(typeId);
}

TableView *FileTableModel::table() const {
    return d->table;
}

int FileTableModel::rowCount(QModelIndex const &) const {
    return d->rowCount();
}

int FileTableModel::columnCount(QModelIndex const &) const {
    return d->columnCount();
}

QVariant FileTableModel::data(QModelIndex const &index, int role) const {
    return d->data(index, role);
}

QVariant FileTableModel::headerData(int section, Qt::Orientation orientation, int role) const {
    return d->headerData(section, orientation, role);
}

void FileTableModel::sort(int column, Qt::SortOrder order) {
    d->sort(column, order, this);
}

} // namespace greu::grylizo
