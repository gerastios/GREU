#include "MainWindow.h"
#include <QHBoxLayout>
#include <QMenuBar>
#include <QMouseEvent>
#include <QStatusBar>
#include <QToolBar>
#include <QTreeView>
#include "AbstractAttributeEditor.h"
#include "Actions.h"
#include "AlbumTable.h"
#include "Application.h"
#include "DirectoryTree.h"
#include "Dispatcher.h"
#include "DuplicateTool.h"
#include "FileEditor.h"
#include "FileTable.h"
#include "FindTool.h"
#include "MainEditor.h"
#include "NormalizeTool.h"
#include "Settings.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/utils.h"
#include "greutag/include/AudioCollection.h"
#include "greutag/include/Directory.h"
#include "greutag/include/File.h"
#include "greutag/include/Tag.h"
#include "greuui/include/FileSystemModel.h"
#include "greuui/include/LogViewer.h"
#include "greuui/include/MediaPlayer.h"
#include "greuui/include/Splitter.h"
#include "greuui/include/TableView.h"
#include "greuui/include/connect.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct MainWindow::Detail {
    unique_ptr<Actions> actions;
    GuardedPtr<Splitter> mainLogSplitter, treeEditorFilesSplitter;
    unique_ptr<ui::LogViewer> logViewer;
    GuardedPtr<QTabWidget> treeTabs;
    unique_ptr<AlbumTable> albumTable;
    unique_ptr<DirectoryTree> directoryTree;
    unique_ptr<FileEditor> fileEditor;
    unique_ptr<FileTable> fileTable;
    GuardedPtr<QStatusBar> status;
    GuardedPtr<MediaPlayer> player;
    ScopedConnection fileContainerChanged, folderRefreshed, albumListChanged, containerSet, fileSet,
        aboutToDeleteFile;

    void setActiveAlbum(Album *album) { albumTable->setActiveAlbum(album); }

    void setupUi() {
        GREU_M->resize(1200, 800);
        GREU_M->setWindowIcon(QIcon(QPixmap(":/images/images/grylizo.png")));
        setTitle();

        mainLogSplitter = new Splitter(Qt::Vertical, "mainLogSplitter");
        treeEditorFilesSplitter = new Splitter(Qt::Horizontal, "treeEditorFilesSplitter");
        mainLogSplitter->addWidget(treeEditorFilesSplitter.get());
        showLog();
        showTrees();
        showEditor();
        showFileTable();
        status = new QStatusBar(GREU_M);
        GREU_M->setStatusBar(status.get());

        actions = make_unique<Actions>();

        connectTabSelect();
        connectContainerSelect();
        connectFileSelect();
        connectEditor();

        GREU_M->setCentralWidget(mainLogSplitter.get());
    }

    void setActiveFile(File *file) {
        if (!file)
            return;
        auto tab = treeTabs->currentIndex();
        if (tab == tabAlbums) {
            albumTable->setActiveAlbum(file->album());
            albumTable->update();
            fileTable->setFileContainer(file->album());
        } else {
            directoryTree->selectPath(file->directory()->path());
            fileTable->setFileContainer(file->directory());
        }
        fileTable->sortByHeaderInfo();
        fileTable->selectFile(file, true);
        fileEditor->setFile(file);
    }

    void endEditing() { fileEditor->commit(); }

    void saveOtherSettings() { saveLast(fileEditor->file()); }

    void setStatusMessage(QString const &text) { status->showMessage(text, 5000); }

    void gotoFile(File *file) {
        fileEditor->setFile(file);
        if (!file)
            return;
        switch (fileTable->displayType()) {
        case FileTable::DisplayType::Album:
            fileTable->setFileContainer(file->album());
            break;
        case FileTable::DisplayType::Directory:
            fileTable->setFileContainer(file->directory());
            break;
        default:
            fileTable->setFileContainer(nullptr);
            break;
        }
        auto container = fileTable->fileContainer();
        if (container) {
            auto rowc = container->indexOf(file);
            if (rowc) {
                fileTable->table()->selectRow(rowc.value());
                fileTable->table()->scrollTo(fileTable->index(rowc.value(), 0),
                                             QAbstractItemView::PositionAtTop);
            }
        }
    }

    void relativeFile(int step) {
        endEditing();
        auto container = fileTable->fileContainer();
        if (!container)
            return;
        auto file = fileEditor->file();
        if (!file)
            return;
        auto currentIdxc = container->indexOf(file);
        if (!currentIdxc)
            return;
        int64_t idx = currentIdxc.value() + step;
        if (idx < 0 || idx >= (int64_t) container->size())
            return;
        fileEditor->setFile(container->at((size_t) idx));
    }

    void setTitle(File *file = nullptr) {
        if (file)
            GREU_M->setWindowTitle(QObject::tr("%2 - %3 Editor (%1 files)")
                                       .arg(QString::number(GREU_D->audioCollection()->size()),
                                            file->tag()->completeFileName())
                                       .arg(Application::instance()->winTitle()));
        else
            GREU_M->setWindowTitle(QObject::tr("%2 Editor (%1 files)")
                                       .arg(QString::number(GREU_D->audioCollection()->size()))
                                       .arg(Application::instance()->winTitle()));
    }

    void showLog() {
        logViewer = make_unique<LogViewer>(&GREU_D->log());
        mainLogSplitter->addWidget(logViewer->setupUi());
    }

    void showTrees() {
        treeTabs = new QTabWidget(GREU_M);
        treeTabs->setObjectName("TreeTabs");
        treeEditorFilesSplitter->addWidget(treeTabs.get());
        showAlbumTree();
        showDirectoryTree();
    }

    void showAlbumTree() {
        albumTable = make_unique<AlbumTable>();
        treeTabs->addTab(albumTable->setupUi(), QObject::tr("Albums"));
    }

    void showDirectoryTree() {
        directoryTree = make_unique<DirectoryTree>();
        treeTabs->addTab(directoryTree->setupUi("Folders"), QObject::tr("Folders"));
    }

    void showEditor() {
        fileEditor = make_unique<FileEditor>();
        treeEditorFilesSplitter->addWidget(fileEditor->setupUi());
    }

    void showFileTable() {
        fileTable = make_unique<FileTable>();
        treeEditorFilesSplitter->addWidget(fileTable->setupUi());
    }

    void connectTabSelect() {
        connectCurrent(treeTabs.get(), [this](auto index) {
            if (!fileEditor || GREU_D->noUiUpdate())
                return;
            auto file = fileEditor->file();
            if (file) {
                if (index == tabAlbums) {
                    fileTable->setFileContainer(file->album());
                    albumTable->setActiveAlbum(file->album());
                } else {
                    fileTable->setFileContainer(file->directory());
                    directoryTree->selectPath(file->directory()->path());
                }
                fileEditor->setFile(file);
            }
        });
    }

    void connectContainerSelect() {
        ui::connectDoubleClicked(albumTable->table(), [this](QModelIndex const &index) {
            if (GREU_D->noUiUpdate())
                return;
            auto album = GREU_D->audioCollection()->albumAt(index.row());
            if (album)
                setFileContainer(album);
        });
        ui::connectClicked(albumTable->table(), [this](QModelIndex const &index) {
            if (GREU_D->noUiUpdate())
                return;
            auto album = GREU_D->audioCollection()->albumAt(index.row());
            if (album)
                setFileContainer(album);
        });
        ui::connectClicked(directoryTree->treeView(), [this](QModelIndex const &index) {
            if (GREU_D->noUiUpdate())
                return;
            auto f = directoryTree->model()->fileInfo(index);
            if (!f.isDir())
                return;
            Modifiable::enableTracking(false);
            auto nf = GREU_D->audioCollection()->directory(f.canonicalFilePath());
            if (nf)
                setFileContainer(nf);
            Modifiable::enableTracking(true);
        });
        fileContainerChanged = GREU_D->audioCollection()->fileContainerChanged().connect(
            [this](FileContainer *container) {
                if (GREU_D->noUiUpdate())
                    return;
                if (fileTable->displayType() == FileTable::DisplayType::Album) {
                    Modifiable::enableTracking(false);
                    fileTable->setFileContainer(container);
                    Modifiable::enableTracking(true);
                }
            });
        folderRefreshed = GREU_D->audioCollection()->directoryRefreshed().connect(
            [this](Directory *directory) {
                if (GREU_D->noUiUpdate())
                    return;
                if (fileTable->displayType() == FileTable::DisplayType::Directory)
                    fileTable->setFileContainer(directory);
            });
        albumListChanged = GREU_D->audioCollection()->albumListChanged().connect([] {
            if (GREU_D->noUiUpdate())
                return;
            GREU_M->update();
        });
        containerSet = fileTable->containerSet().connect([this](FileContainer *container) {
            if (GREU_D->noUiUpdate())
                return;
            if (fileTable->displayType() == FileTable::DisplayType::Album)
                GREU_M->setActiveAlbum(safeCast<Album>(container));
            else if (fileTable->displayType() == FileTable::DisplayType::Directory)
                directoryTree->selectPath(safeCast<Directory>(container)->path());
        });
    }

    void connectEditor() {
        fileSet = fileEditor->fileSet().connect([this](File *file) {
            if (GREU_D->noUiUpdate())
                return;
            setTitle(file);
            fileTable->selectFile(file, false);
        });
        aboutToDeleteFile = GREU_D->audioCollection()->aboutToDeleteFile().connect(
            [this](File *file) {
                if (GREU_D->noUiUpdate())
                    return;
                if (fileEditor->file() == file)
                    fileEditor->setFile(nullptr);
            });
    }

    void connectFileSelect() {
        ui::connectClicked(fileTable->table(), [this](QModelIndex const &index) {
            if (GREU_D->noUiUpdate())
                return;
            fileEditor->setFile(fileTable->fileAt((index.row())));
        });
        ui::connectDoubleClicked(fileTable->table(), [this](QModelIndex const &index) {
            if (GREU_D->noUiUpdate())
                return;
            fileEditor->setFile(fileTable->fileAt((index.row())));
        });
    }

    void setFileContainer(FileContainer *container) {
        if (fileTable->fileContainer() != container) {
            fileTable->setFileContainer(container);
            fileTable->table()->clearSelection();
            auto fst = *container->begin();
            if (fst)
                fileEditor->setFile(fst);
        }
    }

    void restoreLast() {
        auto stg = Settings::instance();
        auto fileName = stg->lastFileName();
        auto path = stg->lastFilePath();
        File *file = nullptr;
        GREU_D->audioCollection()->forEach([&file, fileName, path](auto f) {
            if (f->tag()->completeFileName() == fileName && f->tag()->filePath() == path)
                file = f;
        });
        if (file)
            setActiveFile(file);
    }

    void saveLast(File *file) {
        auto stg = Settings::instance();
        stg->setLastFileName(file ? file->tag()->completeFileName() : QString());
        stg->setLastFilePath(file ? file->tag()->filePath() : QString());
    }
};

MainWindow::MainWindow()
    : d(GREU_MKDET()) {
    d->player = new MediaPlayer;
}

FileEditor *MainWindow::fileEditor() const {
    return d->fileEditor.get();
}

FileTable *MainWindow::fileTable() const {
    return d->fileTable.get();
}

AlbumTable *MainWindow::albumTable() const {
    return d->albumTable.get();
}

DirectoryTree *MainWindow::directoryTree() const {
    return d->directoryTree.get();
}

QTabWidget *MainWindow::treeTabs() const {
    return d->treeTabs.get();
}

MediaPlayer *MainWindow::player() const {
    return d->player.get();
}

void MainWindow::setActiveAlbum(Album *album) {
    d->setActiveAlbum(album);
}

void MainWindow::setupUiInt() {
    d->setupUi();
}

void MainWindow::setActiveFile(File *file) {
    d->setActiveFile(file);
}

void MainWindow::update() {
    d->albumTable->update();
    d->fileTable->setFileContainer(d->fileTable->fileContainer(), true);
    d->fileEditor->update();
}

void MainWindow::endEditing() {
    d->endEditing();
}

void MainWindow::saveOtherSettings() {
    d->saveOtherSettings();
}

void MainWindow::restoreOtherSettings() {
    d->restoreLast();
}

void MainWindow::setStatusMessage(QString const &text) {
    d->setStatusMessage(text);
}

void MainWindow::gotoFile(File *f) {
    d->gotoFile(f);
}

void MainWindow::relativeFile(int step) {
    d->relativeFile(step);
}

} // namespace greu::grylizo
