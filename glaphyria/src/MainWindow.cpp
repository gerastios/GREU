#include "MainWindow.h"
#include <QFileSystemModel>
#include <QProcess>
#include <QTreeView>
#include "Application.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/MimeDataVector.h"
#include "greucore/include/vectorUtils.h"
#include "greuui/include/Splitter.h"
#include "greuui/include/utils.h"
#include <vector>

using namespace std;

namespace greu::glaphyria {

using namespace core;
using namespace ui;

struct MainWindow::Detail {
    struct Model : public QFileSystemModel {
        virtual QMimeData *mimeData(QModelIndexList const &indexes) const override {
            auto res = new MimeDataVector<QString>();
            for (auto i : indexes) {
                auto fn = fileInfo(i).filePath();
                if (!contains(res->data, fn))
                    res->data.push_back(fn);
            }
            return res;
        }

        virtual Qt::ItemFlags flags(QModelIndex const &index) const override {
            auto def = QFileSystemModel::flags(index);
            if (index.isValid())
                return Qt::ItemIsDragEnabled | Qt::ItemIsDropEnabled | def;
            return def;
        }

        virtual bool canDropMimeData(
            QMimeData const *data, Qt::DropAction, int, int, QModelIndex const &) const override {
            auto m = dynamic_cast<MimeDataVector<QString> const *>(data);
            if (!m)
                return false;
            return true;
        }

        virtual Qt::DropActions supportedDropActions() const override {
            return Qt::CopyAction | Qt::MoveAction;
        }

        virtual Qt::DropActions supportedDragActions() const override {
            return Qt::CopyAction | Qt::MoveAction;
        }

        virtual bool dropMimeData(
            QMimeData const *data, Qt::DropAction, int, int, QModelIndex const &parent) override {
            auto m = dynamic_cast<MimeDataVector<QString> const *>(data);
            if (!m)
                return false;
            if (m->data.size() == 0)
                return false;
            auto targetFile = fileInfo(parent);
            QDir target;
            if (targetFile.isDir()) {
                target = QDir(targetFile.filePath());
            } else {
                target = QDir(targetFile.dir());
            }
            layoutAboutToBeChanged();
            for (auto &s : m->data) {
                auto source = QFileInfo(s).filePath();
                auto sourceFileName = QFileInfo(s).fileName();
                vector<QString> args;
                args.push_back(source);
                args.push_back(target.path() + "/" + sourceFileName);
                QProcess proc;
                proc.start("/usr/bin/ln", ranges::to<QStringList>(args));
                proc.waitForFinished();
            }
            layoutChanged();
            return true;
        }
    };

    struct View : public QTreeView {
        GuardedPtr<Model> model;

        View(QString const &name, QString const &path)
            : model(new Model) {
            setObjectName(name);
            model->setObjectName(name + "Model");
            model->setRootPath(QDir::rootPath());
            setModel(model.get());
            setRootIndex(model->index(path));
            setDragDropMode(DragDrop);
            viewport()->setAcceptDrops(true);
            setDropIndicatorShown(true);
            setSelectionMode(ExtendedSelection);
        }
    };

    GuardedPtr<Splitter> mainSplitter;
    GuardedPtr<View> left, right;

    void setupUi() {
        mainSplitter = new Splitter(Qt::Horizontal, "mainSplitter");
        addNewWidget(left, "left", "/", mainSplitter.get());
        addNewWidget(right, "right", "/", mainSplitter.get());
    }
};

MainWindow::MainWindow()
    : d(GREU_MKDET()) {}

void MainWindow::setupUiInt() {
    d->setupUi();
    setCentralWidget(d->mainSplitter.get());
    setWindowTitle(Application::instance()->winTitle());
}

} // namespace greu::glaphyria
