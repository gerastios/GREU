#include "FileListTable.h"
#include <QContextMenuEvent>
#include <QFileSystemModel>
#include <QHeaderView>
#include <QMenu>
#include <QTreeView>
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/MimeDataVector.h"
#include "greucore/include/vectorUtils.h"
#include "greuui/include/Action.h"
#include <functional>
#include <vector>
#include "greuui/include/connect.h"

using namespace std;

namespace greu::geranos {

using namespace core;
using namespace ui;

struct FileListTable::Detail {
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
                return Qt::ItemIsDragEnabled | def;
            return def;
        }
    };

    struct Table : public QTreeView {
        GuardedPtr<Model> model;
        GuardedPtr<QMenu> ctxMenu;
        vector<GuardedPtr<Action>> actions;
        MediaSig trackInfoRequested;
        StringSig externPlayRequested, backupRequested, trackChapters, editChapters;

        Table(QString const &path)
            : model(new Model) {
            ctxMenu = new QMenu;
            setObjectName("fileTable");
            model->setObjectName("fileListTableModel");
            model->setRootPath(QDir::rootPath());
            setModel(model.get());
            setRootIndex(model->index(path));
            setDragDropMode(DragOnly);
            setSelectionMode(ExtendedSelection);
        }

        void contextMenuEvent(QContextMenuEvent *event) override {
            ctxMenu->clear();
            actions.clear();
            auto idx = indexAt(event->pos());
            if (idx.isValid() && model->fileInfo(idx).isFile()) {
                addContextAction(tr("Show &tracks"), [this, idx] {
                    trackInfoRequested(Media{{model->fileInfo(idx).filePath()}});
                });
                addContextAction(tr("&Backup track"), [this, idx] {
                    auto fp = model->fileInfo(idx).filePath();
                    if (model->fileInfo(idx).suffix().toUpper() == "ISO")
                        backupRequested(fp);
                });
                addContextAction(tr("&Show chapters of track"), [this, idx] {
                    auto fp = model->fileInfo(idx).filePath();
                    if (model->fileInfo(idx).suffix().toUpper() == "ISO")
                        trackChapters(fp);
                });
                addContextAction(tr("Edit chapters"), [this, idx] {
                    auto fp = model->fileInfo(idx).filePath();
                    Media f{{fp}};
                    if (f.isValid())
                        editChapters(fp);
                });
                addContextAction(tr("Play &externally"), [this, idx] {
                    auto fp = model->fileInfo(idx).filePath();
                    Media f{{fp}};
                    if (f.isValid())
                        externPlayRequested(fp);
                });
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
    };

    GuardedPtr<Table> table;
    QString path;

    Detail(QString const &path)
        : path(path) {}

    QWidget *setupUi() {
        table = new Table(path);
        return table.get();
    }
};

FileListTable::FileListTable(QString const &path)
    : d(GREU_MKDET(path)) {}

QWidget *FileListTable::setupUi() {
    return d->setupUi();
}

StringSig &FileListTable::editChapters() const {
    return d->table->editChapters;
}

StringSig &FileListTable::externPlayRequested() const {
    return d->table->externPlayRequested;
}

FileListTable::MediaSig &FileListTable::trackInfoRequested() const {
    return d->table->trackInfoRequested;
}

StringSig &FileListTable::backupRequested() const {
    return d->table->backupRequested;
}

StringSig &FileListTable::trackChapters() const {
    return d->table->trackChapters;
}

} // namespace greu::geranos
