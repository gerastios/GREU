#include "AlbumTable.h"
#include <QBuffer>
#include <QHeaderView>
#include "Application.h"
#include "Dispatcher.h"
#include "FileEditor.h"
#include "MainEditor.h"
#include "MainWindow.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/MimeDataVector.h"
#include "greutag/include/Album.h"
#include "greutag/include/AlbumChanger.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/AttributeTypes.h"
#include "greutag/include/AudioCollection.h"
#include "greutag/include/File.h"
#include "greuui/include/TableView.h"

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct AlbumTable::Detail {
    struct Model : public QAbstractTableModel {
        AlbumTable::Detail *that;
        tag::AudioCollection *collection;
        TableView *table;
        bool isSorted = false;
        Qt::SortOrder order;
        int sortColumn;
        static int const START_COLUMNS = __LINE__ + 2;
        enum Columns : int {
            Title,
            Artist,
            TrackCount,
            Image,
        };
        static int const COLUMNS_COUNT = __LINE__ - START_COLUMNS - 1;

        Model(AlbumTable::Detail *that, AudioCollection *collection, TableView *table)
            : QAbstractTableModel(table)
            , that(that)
            , collection(collection)
            , table(table) {
            this->collection->modifiedChanged().connect([this](auto) {
                if (GREU_D->noUiUpdate())
                    return;
                if (isSorted) {
                    sort(sortColumn, order);
                } else {
                    layoutAboutToBeChanged();
                    layoutChanged();
                }
            });
        }

        virtual int rowCount(QModelIndex const &) const override {
            return collection->albumCount();
        }

        virtual int columnCount(QModelIndex const &) const override { return COLUMNS_COUNT; }

        virtual QVariant data(QModelIndex const &index, int role) const override {
            if (!index.isValid() || collection->albumCount() == 0) {
                return QVariant();
            }
            auto sz = table->rowHeight(index.row());
            if ((size_t) index.row() + 1 > collection->albumCount())
                return QVariant();
            auto album = collection->albumAt(index.row());
            assert(album && "No album in this row");
            if (role == Qt::ItemDataRole::ForegroundRole && album->isIncomplete()) {
                return QBrush(Qt::red);
            }
            if (role == Qt::ItemDataRole::DecorationRole && index.column() == int(Columns::Image)) {
                auto i = album->image();
                return i.isNull()
                           ? QImage()
                           : album->image().scaled(sz, sz, Qt::AspectRatioMode::KeepAspectRatio);
            }
            if (role == Qt::ItemDataRole::ToolTipRole && index.column() == int(Columns::Image)) {
                QByteArray ba;
                QBuffer bu(&ba);
                bu.open(QIODevice::WriteOnly);
                album->image().save(&bu, "PNG");
                return QString("<img src='data:image/jpg;base64, %0'><br>%1 - %2")
                    .arg(QString(ba.toBase64()), album->artist(), album->title());
            }
            if (role != Qt::ItemDataRole::DisplayRole) {
                return QVariant();
            }
            switch (index.column()) {
            case int(Columns::Title):
                return album->title();
            case int(Columns::Artist):
                return album->artist();
            case int(Columns::TrackCount):
                return (uint) album->trackCount();
            case int(Columns::Image):
                return "";
            }
            return QVariant();
        }

        virtual QVariant headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const override {
            if (role == Qt::DisplayRole && orientation == Qt::Horizontal) {
                switch (section) {
                case int(Columns::Title):
                    return tag::AttributeTypes::type(tag::AttributeTypes::albumTitle())->shortUpper();
                case int(Columns::Artist):
                    return tag::AttributeTypes::type(tag::AttributeTypes::albumArtist())
                        ->shortUpper();
                case int(Columns::TrackCount):
                    return tag::AttributeTypes::type(tag::AttributeTypes::trackNumber())
                        ->shortUpper();
                case int(Columns::Image):
                    return tag::AttributeTypes::type(tag::AttributeTypes::images())->shortUpper();
                }
            }
            return QVariant();
        }

        virtual bool canDropMimeData(
            QMimeData const *data, Qt::DropAction, int, int, QModelIndex const &) const override {
            auto m = dynamic_cast<core::MimeDataVector<tag::File *> const *>(data);
            if (!m)
                return false;
            return true;
        }

        virtual bool dropMimeData(
            QMimeData const *data, Qt::DropAction, int, int, QModelIndex const &parent) override {
            auto m = dynamic_cast<core::MimeDataVector<tag::File *> const *>(data);
            if (!m)
                return false;
            if (m->data.size() == 0)
                return false;
            auto row = parent.row();
            if (row == -1)
                return false;
            auto album = collection->albumAt(row);
            if (!album)
                return false;
            AlbumChanger changer{tag::Album::Key{.title = album->title(), .artist = album->artist()},
                                 m->data,
                                 collection};
            changer.setMainFile(m->data[0]);
            GREU_M->fileEditor()->mainEditor()->albumChange(&changer);
            return true;
        }

        virtual Qt::DropActions supportedDragActions() const override {
            return Qt::CopyAction | Qt::MoveAction;
        }

        virtual Qt::ItemFlags flags(QModelIndex const &index) const override {
            auto def = QAbstractTableModel::flags(index);
            if (index.isValid())
                return Qt::ItemIsDropEnabled | def;
            return def;
        }

        virtual void sort(int column, Qt::SortOrder order) override {
            if (collection->albumCount() == 0)
                return;
            layoutAboutToBeChanged();
            isSorted = true;
            this->order = order;
            sortColumn = column;
            switch (column) {
            case int(Columns::Title):
                collection->sortAlbums(order, tag::AudioCollection::Sort::title);
                break;
            case int(Columns::Artist):
                collection->sortAlbums(order, tag::AudioCollection::Sort::artist);
                break;
            case int(Columns::TrackCount):
                collection->sortAlbums(order, tag::AudioCollection::Sort::trackCount);
                break;
            default:
                return;
            }
            layoutChanged();
        }

        int rowOf(tag::Album const *album) const { return collection->indexOf(album); }

        void update() {
            layoutAboutToBeChanged();
            layoutChanged();
        }
    };

    GuardedPtr<Model> model;
    GuardedPtr<TableView> table;

    QWidget *setupUi() {
        table = new TableView("AlbumTable");
        model = new Model(this, GREU_D->audioCollection(), table.get());
        table->setSelectionMode(QTableView::SingleSelection);
        table->setAcceptDrops(true);
        table->setDropIndicatorShown(true);
        table->setDragDropMode(QAbstractItemView::DropOnly);
        table->setModel(model.get());
        table->horizontalHeader()->setMinimumSectionSize(20);
        return table.get();
    }

    void setActiveAlbum(Album const *album, bool airSignal = false) {
        table->clearSelection();
        auto r = model->rowOf(album);
        if (r >= 0) {
            table->selectRow(r);
            table->scrollTo(model->index(r, 0));
            if (airSignal)
                table->clicked(model->index(r, 0));
        }
    }

    void update() { model->update(); }
};

AlbumTable::AlbumTable()
    : d(GREU_MKDET()) {}

QWidget *AlbumTable::setupUi() {
    return d->setupUi();
}

TableView *AlbumTable::table() const {
    return d->table.get();
}

void AlbumTable::setActiveAlbum(Album const *album) {
    d->setActiveAlbum(album);
}

void AlbumTable::update() {
    d->update();
}

} // namespace greu::grylizo
