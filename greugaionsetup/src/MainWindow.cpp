#include "MainWindow.h"
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QMenu>
#include <QScrollArea>
#include <QSplitter>
#include <QTreeView>
#include <QVBoxLayout>
#include "Actions.h"
#include "Dispatcher.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/utils.h" // IWYU pragma: keep
#include "greugaion/include/PresentationWidget.h"
#include "greumovies/include/File.h"
#include "greumovies/include/FilesTable.h"
#include "greumovies/include/Settings.h"
#include "greusql/include/Connection.h"
#include "greuui/include/Action.h"
#include "greuui/include/Application.h"
#include "greuui/include/FileSystemModel.h"
#include "greuui/include/connect.h"

using namespace std;

namespace greu::gaion::setup {

using namespace core;
using namespace sql;
using namespace ui;
using namespace movies;
using namespace gaion;

struct MainWindow::Detail {
    struct Model : public FileSystemModel {
        virtual QVariant data(QModelIndex const &index, int role) const override {
            if (role != Qt::ItemDataRole::ForegroundRole || !index.isValid())
                return FileSystemModel::data(index, role);
            auto f = fileInfo(index);
            if (!f.isFile())
                return FileSystemModel::data(index, role);
            if (File::isUsed(f.absoluteFilePath()))
                return QColorConstants::Svg::crimson;
            return {};
        }
    };

    struct FileTreeView : public QTreeView {
        GuardedPtr<QMenu> ctxMenu;
        vector<GuardedPtr<Action>> actions;
        FileInfoSig assignMovieRequested, assignEpisodeRequested, assignOwnMovieRequested,
            unassignFileRequested, playFileRequested, updateFileRequested;
        ModelIndexSig selectionChanged_;
        GuardedPtr<Model> model;

        FileTreeView() {
            setObjectName("SetupFileTree");
            header()->setObjectName("SetupFileTreeHeader");
            ctxMenu = new QMenu;
            model = new Model;
            setSelectionMode(SelectionMode::SingleSelection);
        }

        virtual void contextMenuEvent(QContextMenuEvent *event) override {
            ctxMenu->clear();
            actions.clear();
            auto idx = indexAt(event->pos());
            if (idx.isValid() && model->fileInfo(idx).isFile()) {
                addContextAction(tr("Assign &movie"),
                                 [this, idx] { assignMovieRequested(model->fileInfo(idx)); });
                addContextAction(tr("Assign TV &episode"),
                                 [this, idx] { assignEpisodeRequested(model->fileInfo(idx)); });
                addContextAction(tr("Assign &own movie"),
                                 [this, idx] { assignOwnMovieRequested(model->fileInfo(idx)); });
                addContextAction(tr("&Update"),
                                 [this, idx] { updateFileRequested(model->fileInfo(idx)); });
                addContextAction(tr("Unassign &file"),
                                 [this, idx] { unassignFileRequested(model->fileInfo(idx)); });
                addContextAction(tr("&Play"),
                                 [this, idx] { playFileRequested(model->fileInfo(idx)); });
                ctxMenu->popup(event->globalPos());
                event->accept();
            }
        }

        void addContextAction(QString const &text, function<void()> func) {
            GuardedPtr<Action> act = new Action(text);
            ui::connect(act.get(), func);
            ctxMenu->addAction(act.get());
            actions.push_back(std::move(act));
        }

        ModelIndexSig &selectionChanged() { return selectionChanged_; }

    protected:
        virtual void selectionChanged(QItemSelection const &selected,
                                      QItemSelection const &) override {
            if (selected.indexes().size() > 0)
                selectionChanged_(selected.indexes()[0]);
            else
                selectionChanged_({});
        }
    };

    GuardedPtr<QSplitter> mainSplitter;
    GuardedPtr<QWidget> rightWidget;
    GuardedPtr<QVBoxLayout> rightLayout;
    PresentationWidget *mediumWidget = nullptr;
    GuardedPtr<FileTreeView> fileTree;
    GuardedPtr<Model> fileModel;
    FileInfoSig fileSelected;

    void setupUi(MainWindow *that) {
        mainSplitter = new QSplitter;
        that->setCentralWidget(mainSplitter.get());
        mainSplitter->setObjectName("mainsplitter");
        mainSplitter->setOrientation(Qt::Horizontal);
        fileTree = new FileTreeView;
        mainSplitter->addWidget(fileTree.get());
        rightWidget = new QWidget;
        mainSplitter->addWidget(rightWidget.get());
        rightLayout = new QVBoxLayout;
        rightWidget->setLayout(rightLayout.get());
        fileTree->setObjectName("filetree");
        fileModel = new Model;
        auto root = movies::Settings::instance()->pathPrefix();
        fileModel->setRootPath(root);
        fileTree->setModel(fileModel.get());
        fileTree->model = fileModel.get();
        fileTree->setRootIndex(fileModel->index(root));
        ui::connectClicked(fileTree.get(), [this](auto idx) { selectedChanged(idx); });
        fileTree->selectionChanged().connect([this](auto idx) { selectedChanged(idx); });
    }

    void selectedChanged(QModelIndex const &idx) { fileSelected(fileTree->model->fileInfo(idx)); }
};

MainWindow::MainWindow()
    : d(GREU_MKDET()) {
    setObjectName("mainwindowsetup");
    setWindowTitle((Application::instance()->winTitle() + ": setup %1")
                       .arg(sql::Connection::instance()->schema()));
}

void MainWindow::setMediumWidget(PresentationWidget *w) {
    if (w == d->mediumWidget)
        return;
    if (d->mediumWidget) {
        d->rightLayout->removeWidget(d->mediumWidget->widget());
        d->mediumWidget->widget()->hide();
    }
    d->mediumWidget = w;
    if (w)
        d->rightLayout->addWidget(d->mediumWidget->widget());
}

PresentationWidget *MainWindow::mediumWidget() const {
    return d->mediumWidget;
}

FileInfoSig &MainWindow::updateFileRequested() const {
    return d->fileTree->updateFileRequested;
}

FileInfoSig &MainWindow::fileSelected() const {
    return d->fileSelected;
}

FileInfoSig &MainWindow::assignEpisdoeRequested() const {
    return d->fileTree->assignEpisodeRequested;
}

FileInfoSig &MainWindow::assignMovieRequested() const {
    return d->fileTree->assignMovieRequested;
}

FileInfoSig &MainWindow::unassignFileRequested() const {
    return d->fileTree->unassignFileRequested;
}

FileInfoSig &MainWindow::assignOwnMovieRequested() const {
    return d->fileTree->assignOwnMovieRequested;
}

FileInfoSig &MainWindow::playFileRequested() const {
    return d->fileTree->playFileRequested;
}

FileSystemModel *MainWindow::fileModel() const {
    return d->fileModel.get();
}

QTreeView *MainWindow::fileTree() const {
    return d->fileTree.get();
}

vector<QFileInfo> MainWindow::selectedFiles() const {
    auto indexes = d->fileTree->selectionModel()->selectedRows();
    vector<QFileInfo> res;
    for (auto &i : indexes)
        res.push_back(d->fileModel->fileInfo(i));
    return res;
}

void MainWindow::setupUiInt() {
    d->setupUi(this);
}

} // namespace greu::gaion::setup
