#include "FileTable.h"
#include <QBuffer>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QLabel>
#include <QTabWidget>
#include <QVBoxLayout>
#include "Application.h"
#include "Dispatcher.h"
#include "FileTableModel.h"
#include "MainWindow.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/MimeDataVector.h"
#include "greucore/include/signalUtils.h"
#include "greucore/include/utils.h"
#include "greucore/include/vectorUtils.h"
#include "greutag/include/Album.h"
#include "greutag/include/AttributeTypes.h"
#include "greutag/include/AudioCollection.h"
#include "greutag/include/Directory.h"
#include "greutag/include/File.h"
#include "greuui/include/Action.h"
#include "greuui/include/TableView.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct FileTable::Detail {
    struct Model : public FileTableModel {
        vector<ScopedConnection> connsModified;
        ContainerSig containerSet;
        GuardedPtr<Action> actDelete;
        ScopedConnection connAboutDelete;

        Model(TableView *table)
            : FileTableModel(table,
                             {AttributeTypes::fileName(),
                              AttributeTypes::songTitle(),
                              AttributeTypes::songArtist(),
                              AttributeTypes::trackNumber(),
                              AttributeTypes::discNumber(),
                              AttributeTypes::albumTitle(),
                              AttributeTypes::albumArtist(),
                              AttributeTypes::images()}) {
            actDelete = new Action(QObject::tr("&Delete file"));
            connAboutDelete = GREU_D->audioCollection()->aboutToDeleteAlbum().connect(
                [this](class Album *album) {
                    if (album == fileHolder())
                        setFileHolder(nullptr);
                });
            table->addContextAction(actDelete.get(), [this](int row, int, QAbstractTableModel *) {
                auto files = fileHolder();
                if (!files)
                    return;
                if (row < 0 || row >= int(files->size()))
                    return;
                GREU_D->audioCollection()->deleteFile(files->at(row));
            });
        }

        virtual void setFileHolderPostProc() override {
            core::disconnect(connsModified);
            connsModified.clear();
            auto container = dynamic_cast<tag::FileContainer *>(fileHolder());
            if (container) {
                for (auto f = container->begin(); f != container->end(); ++f) {
                    ScopedConnection conn = (*f)->modifiedChanged().connect([this](auto) {
                        if (GREU_D->noUiUpdate())
                            return;
                        layoutAboutToBeChanged();
                        layoutChanged();
                    });
                    connsModified.push_back(std::move(conn));
                }
            }
            containerSet(container);
        }

        virtual QMimeData *mimeData(QModelIndexList const &indexes) const override {
            core::MimeDataVector<tag::File *> *result = new core::MimeDataVector<tag::File *>();
            std::vector<int> rows;
            for (auto i : indexes) {
                if (!core::contains(rows, i.row()))
                    rows.push_back(i.row());
            }
            for (auto r : rows)
                result->data.push_back(fileHolder()->at(r));
            return result;
        }

        virtual Qt::ItemFlags flags(QModelIndex const &index) const override {
            auto def = QAbstractTableModel::flags(index);
            if (index.isValid())
                return Qt::ItemIsDragEnabled | def;
            return def;
        }
    };

    GuardedPtr<QWidget> widget;
    GuardedPtr<QLabel> label;
    GuardedPtr<TableView> table;
    GuardedPtr<Model> model;
    GuardedPtr<QVBoxLayout> layout;
    vector<ScopedConnection> connsMod;

    QWidget *setupUi() {
        widget = new QWidget;
        layout = new QVBoxLayout;
        widget->setLayout(layout.get());
        addNewWidget(table, "FileTable", layout.get());
        model = new Model(table.get());
        table->setModel(model.get());
        table->setSelectionMode(QTableView::ExtendedSelection);
        table->setDragEnabled(true);
        table->setAcceptDrops(false);
        table->setDragDropMode(QAbstractItemView::DragOnly);
        addNewWidget(label, layout.get());
        label->setAlignment(Qt::AlignCenter);
        return widget.get();
    }

    void selectFile(File *file, bool airSignal = false) {
        if (!file) {
            table->clearSelection();
            return;
        }
        auto rowc = model->fileHolder()->indexOf(file);
        if (!rowc) {
            table->clearSelection();
            return;
        }
        auto row = rowc.value();
        table->selectRow(row);
        table->scrollTo(model->index(row, 0));
        if (airSignal)
            table->clicked(model->index(row, 0));
    }

    vector<tag::File *> selectedFiles() const {
        auto rows = table->selectionModel()->selectedRows();
        std::vector<tag::File *> res;
        for (auto i : rows) {
            res.push_back(model->fileHolder()->at(i.row()));
        }
        return res;
    }

    void connectModifiables(FileTable *that) {
        core::disconnect(connsMod);
        connsMod.clear();
        auto container = that->fileContainer();
        if (!container)
            return;
        for (auto f = container->begin(); f != container->end(); ++f) {
            ScopedConnection conn = (*f)->modifiedChanged().connect([this](auto) {
                if (GREU_D->noUiUpdate())
                    return;
                model->layoutAboutToBeChanged();
                model->layoutChanged();
            });
            connsMod.push_back(std::move(conn));
        }
    }

    void setLabel(DisplayType type) {
        switch (type) {
        case FileTable::DisplayType::None:
            label->setText("");
            break;
        case FileTable::DisplayType::Album: {
            auto album = safeCast<tag::Album>(model->fileHolder());
            label->setText(QObject::tr("Album %1 - %2").arg(album->artist(), album->title()));
        } break;
        case FileTable::DisplayType::Directory: {
            auto folder = safeCast<tag::Directory>(model->fileHolder());
            label->setText(QObject::tr("Folder %1").arg(folder->path()));
        } break;
        }
    }

    void sortByHeaderInfo() {
        auto h = table->horizontalHeader();
        if (!h->isSortIndicatorShown())
            return;
        model->sort(h->sortIndicatorSection(), h->sortIndicatorOrder());
    }
};

FileTable::FileTable()
    : d(GREU_MKDET()) {}

QWidget *FileTable::setupUi() {
    return d->setupUi();
}

void FileTable::setFileContainer(FileContainer *value, bool always) {
    d->model->setFileHolder(value, always);
    d->setLabel(displayType());
    d->connectModifiables(this);
    auto treeTabs = GREU_M->treeTabs();
    auto tab = treeTabs->currentIndex();
    if (displayType() == DisplayType::Album && tab == MainWindow::tabFolders)
        treeTabs->setCurrentIndex(MainWindow::tabAlbums);
    else if (displayType() == DisplayType::Directory && tab == MainWindow::tabAlbums)
        treeTabs->setCurrentIndex(MainWindow::tabFolders);
}

FileContainer *FileTable::fileContainer() const {
    return safeCast<tag::FileContainer>(d->model->fileHolder());
}

void FileTable::selectFile(File *file, bool emitSignal) {
    d->selectFile(file, emitSignal);
}

FileTable::DisplayType FileTable::displayType() const {
    auto c = d->model->fileHolder();
    if (!c)
        return DisplayType::None;
    if (dynamic_cast<tag::Directory *>(c))
        return DisplayType::Directory;
    return DisplayType::Album;
}

vector<File *> FileTable::selectedFiles() const {
    return d->selectedFiles();
}

TableView *FileTable::table() const {
    return d->table.get();
}

File *FileTable::fileAt(size_t index) const {
    auto fc = d->model->fileHolder();
    if (!fc)
        return nullptr;
    return fc->at(index);
}

QModelIndex FileTable::index(int row, int column) const {
    return d->model->index(row, column);
}

void FileTable::sortByHeaderInfo() {
    d->sortByHeaderInfo();
}

FileTable::ContainerSig &FileTable::containerSet() const {
    return d->model->containerSet;
}

} // namespace greu::grylizo
