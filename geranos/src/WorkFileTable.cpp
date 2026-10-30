#include "WorkFileTable.h"
#include <QAbstractTableModel>
#include <QDropEvent>
#include <QFileInfo>
#include <QHeaderView>
#include <QMenu>
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/MimeDataVector.h"
#include "greucore/include/intUtils.h"
#include "greuui/include/Action.h"
#include "greuui/include/connect.h"
#include <greucore/include/vectorUtils.h>
#include <vector>

using namespace std;

namespace greu::geranos {

using namespace core;
using namespace ui;
using namespace media;

struct WorkFileTable::Detail {
    struct Model : public QAbstractTableModel {
        static int const START_COLUMNS = __LINE__ + 2;
        enum Columns : int {
            Filename,
            VideoTrackCount,
            AudioTrackCount,
            SubtitleTrackCount,
        };
        static int const COLUMNS_COUNT = __LINE__ - START_COLUMNS - 1;

        vector<Media> files;

        int rowCount(QModelIndex const &) const override { return files.size(); }

        int columnCount(QModelIndex const &) const override { return COLUMNS_COUNT; }

        QVariant data(QModelIndex const &index, int role) const override {
            if (role != Qt::ItemDataRole::DisplayRole)
                return QVariant();
            if (!index.isValid() || files.size() == 0)
                return QVariant();
            if ((size_t) index.row() + 1 > files.size())
                return QVariant();
            auto &file = files[index.row()];
            switch (index.column()) {
            case int(Columns::Filename):
                return file.filePath();
            case int(Columns::VideoTrackCount):
                return (uint) file.videoTracks().size();
            case int(Columns::AudioTrackCount):
                return (uint) file.audioTracks().size();
            case int(Columns::SubtitleTrackCount):
                return (uint) file.subtitleTracks().size();
            }
            return QVariant();
        }

        virtual bool canDropMimeData(
            QMimeData const *data, Qt::DropAction, int, int, QModelIndex const &) const override {
            auto m = dynamic_cast<MimeDataVector<QString> const *>(data);
            if (!m)
                return false;
            return true;
        }

        virtual bool dropMimeData(
            QMimeData const *data, Qt::DropAction, int, int, QModelIndex const &parent) override {
            auto m = dynamic_cast<MimeDataVector<QString> const *>(data);
            if (!m)
                return false;
            if (m->data.size() == 0)
                return false;
            layoutAboutToBeChanged();
            for (auto &s : m->data) {
                auto path = QFileInfo(s).path();
                if (files.size() > 0)
                    files.erase(remove_if(files.begin(),
                                          files.end(),
                                          [path](Media &f) { return f.path() != path; }),
                                files.end());
                if (find_if(files.begin(), files.end(), [s](auto &f) { return f.filePath() == s; })
                    == files.end()) {
                    if (parent.row() == -1) {
                        files.push_back({{s}});
                    } else {
                        if (parent.row() > (int) files.size())
                            files.push_back({{s}});
                        else
                            files.insert(files.begin() + parent.row(), {{s}});
                    }
                }
            }
            layoutChanged();
            return true;
        }

        virtual Qt::DropActions supportedDropActions() const override {
            return Qt::CopyAction | Qt::MoveAction;
        }

        virtual Qt::DropActions supportedDragActions() const override {
            return Qt::CopyAction | Qt::MoveAction;
        }

        virtual Qt::ItemFlags flags(QModelIndex const &index) const override {
            auto def = QAbstractTableModel::flags(index);
            return Qt::ItemIsDropEnabled | def;
        }

        virtual QVariant headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const override {
            if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
                return QVariant{};
            switch (section) {
            case int(Columns::Filename):
                return tr("File name");
            case int(Columns::VideoTrackCount):
                return tr("Video tracks");
            case int(Columns::AudioTrackCount):
                return tr("Audio tracks");
            case int(Columns::SubtitleTrackCount):
                return tr("Subtitle tracks");
            }
            return QVariant{};
        }
    };

    GuardedPtr<Model> model;
    GuardedPtr<QMenu> ctxMenu;
    GuardedPtr<Action> actUp, actDown, actRemove, actFirst, actLast;
    int ctxRow;

    void moveFile(int from, int to, WorkFileTable *that) {
        if (!(between(from, 0, (int) model->files.size() - 1)
              && between(to, 0, (int) model->files.size() - 1))
            || from == to)
            return;
        model->layoutAboutToBeChanged();
        core::move(model->files, from, to);
        model->layoutChanged();
        that->selectionModel()->clearSelection();
    }

    void init(WorkFileTable *that) {
        actUp = new Action(QObject::tr("move up"));
        actDown = new Action(QObject::tr("move down"));
        actRemove = new Action(QObject::tr("remove"));
        actFirst = new Action(QObject::tr("at first place"));
        actLast = new Action(QObject::tr("at last place"));
        model = new Model;
        ctxMenu = new QMenu;
        ui::connect(actUp.get(),[this, that] { moveFile(ctxRow, ctxRow - 1, that); });
        ui::connect(actDown.get(), [this, that] { moveFile(ctxRow, ctxRow + 1, that); });
        ui::connect(actFirst.get(), [this, that] { moveFile(ctxRow, 0, that); });
        ui::connect(actLast.get(), [this, that] { moveFile(ctxRow, model->files.size() - 1, that); });
        ui::connect(actRemove.get(), [this, that] {
            if (!between(ctxRow, 0, (int) model->files.size() - 1))
                return;
            model->layoutAboutToBeChanged();
            model->files.erase(model->files.begin() + ctxRow);
            model->layoutChanged();
            that->selectionModel()->clearSelection();
        });
        ctxMenu->addAction(actUp.get());
        ctxMenu->addAction(actDown.get());
        ctxMenu->addAction(actRemove.get());
        ctxMenu->addAction(actFirst.get());
        ctxMenu->addAction(actLast.get());
    }

    void contextMenuEvent(QContextMenuEvent *event, WorkFileTable *that) {
        ctxRow = that->rowAt(event->pos().y());
        if (ctxRow < 0 || ctxRow > (int) model->files.size() - 1)
            return;
        ctxMenu->adjustSize();
        ctxMenu->popup(QCursor::pos());
    }

    optional<Media> media() const {
        if (model->files.size() == 0)
            return nullopt;
        vector<QString> filePaths;
        for (auto &f : model->files) {
            filePaths.push_back(f.filePath());
        }
        return {filePaths};
    }
};

WorkFileTable::WorkFileTable()
    : TableView("workFileTable")
    , d(GREU_MKDET()) {
    d->init(this);
    setModel(d->model.get());
    setSortingEnabled(false);
    setDragDropMode(DropOnly);
}

optional<Media> WorkFileTable::media() const {
    return d->media();
}

void WorkFileTable::clear() {
    d->model->layoutAboutToBeChanged();
    d->model->files.clear();
    d->model->layoutChanged();
}

void WorkFileTable::contextMenuEvent(QContextMenuEvent *event) {
    d->contextMenuEvent(event, this);
}

} // namespace greu::geranos
