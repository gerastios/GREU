#include "DirectoryTree.h"
#include <QMenu>
#include <QMessageBox>
#include <QTreeView>
#include "Dispatcher.h"
#include "FileTable.h"
#include "MainWindow.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/utils.h" // IWYU pragma: keep
#include "greutag/include/AudioCollection.h"
#include "greutag/include/Directory.h"
#include "greuui/include/Action.h"
#include "greuui/include/FileSystemModel.h"
#include "greuui/include/connect.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct DirectoryTree::Detail {
    GuardedPtr<QTreeView> treeView;
    GuardedPtr<FileSystemModel> directoryModel;
    GuardedPtr<Action> actRefresh;
    GuardedPtr<QMenu> ctxMenu;
    QPoint ctxPoint;

    QWidget *setupUi(QString const &name) {
        treeView = new QTreeView;
        treeView->setObjectName(name);
        actRefresh = new Action(QObject::tr("Refresh"));
        ctxMenu = new QMenu;
        directoryModel = new FileSystemModel;
        directoryModel->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
        directoryModel->setRootPath("");
        treeView->setModel(directoryModel.get());
        treeView->setExpanded(directoryModel->index(GREU_D->audioPath()), true);
        treeView->setRootIndex(directoryModel->index(""));
        ctxMenu->addAction(actRefresh.get());
        connectCustomCtxMenu(treeView.get(), [this](QPoint const &point) {
            ctxPoint = point;
            ctxMenu->popup(QCursor::pos());
        });
        ui::connect(actRefresh.get(), [this] {
            auto idx = treeView->indexAt(ctxPoint);
            if (!idx.isValid())
                return;
            auto d = QDir(directoryModel->fileInfo(idx).canonicalFilePath());
            auto coll = GREU_D->audioCollection();
            auto dir = coll->directory(d.path(), true);
            if (dir && dir->isModified()) {
                QMessageBox::information(
                    treeView.get(),
                    QObject::tr("Files were modified"),
                    QObject::tr("Some files in that folder were modified. Please save first."));
                return;
            }
            Modifiable::enableTracking(false);
            coll->refreshDir(d);
            Modifiable::enableTracking(true);
        });
        treeView->scrollTo(directoryModel->index(GREU_D->audioPath()),
                           QAbstractItemView::PositionAtTop);
        return treeView.get();
    }

    void selectPath(QString const &path) {
        auto idx = directoryModel->index(path);
        if (!idx.isValid())
            return;
        treeView->scrollTo(idx, QAbstractItemView::PositionAtTop);
        treeView->selectionModel()->select(idx, QItemSelectionModel::Select);
        if (GREU_M->fileTable()->displayType() != FileTable::DisplayType::Album)
            treeView->clicked(idx);
    }
};

DirectoryTree::DirectoryTree()
    : d(GREU_MKDET()) {}

QWidget *DirectoryTree::setupUi(QString const &name) {
    return d->setupUi(name);
}

QTreeView *DirectoryTree::treeView() const {
    return d->treeView.get();
}

FileSystemModel *DirectoryTree::model() const {
    return d->directoryModel.get();
}

void DirectoryTree::selectPath(QString const &path) {
    d->selectPath(path);
}

} // namespace greu::grylizo
