#include "TableView.h"
#include <QAbstractTableModel>
#include <QContextMenuEvent>
#include <QGuiApplication>
#include <QHeaderView>
#include <QMenu>
#include "Action.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/signalUtils.h"
#include "greucore/include/utils.h"
#include "greucore/include/vectorUtils.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct TableView::Detail {
    unordered_map<Action *, std::pair<TableView::ContextActionFunc, QMetaObject::Connection>> actions;
    vector<TableView::ContextActionRequestFunc> requests;
    vector<QMetaObject::Connection> custConnections;
    vector<Action *> ctxActions;
    GuardedPtr<QMenu> ctxMenu;
    QPoint ctxPoint;

    void contextMenuEvent(QContextMenuEvent *event, TableView *that) {
        ctxPoint = event->pos();
        auto row = that->rowAt(ctxPoint.y());
        if (row < 0 || row >= that->model()->rowCount())
            return;
        auto column = that->columnAt(ctxPoint.x());
        if (column < 0 || column >= that->model()->columnCount())
            return;
        ctxMenu->clear();
        core::disconnect(custConnections);
        custConnections.clear();
        auto model = safeCast<QAbstractTableModel>(that->model());
        for (auto &a : ctxActions) {
            if (a == nullptr) {
                ctxMenu->addSeparator();
            } else {
                ctxMenu->addAction(a);
                QObject::disconnect(actions[a].second);
                actions[a].second = ui::connect(a, [this, a, model, row, column] {
                    actions[a].first(row, column, model);
                });
            }
        }
        for (auto r : requests) {
            auto custActions = r(row, column);
            if (custActions.size() > 0)
                ctxMenu->addSeparator();
            for (auto &a : custActions) {
                ctxMenu->addAction(a.action);
                custConnections.push_back(ui::connect(a.action, [a, model, row, column] {
                    a.function(row, column, model);
                }));
            }
        }
        ctxMenu->popup(QCursor::pos());
    }

    void addContextAction(Action *action, ContextActionFunc function) {
        assert(action && "Invalid action given");
        assert(function && "Invalid function given");
        actions[action] = std::make_pair(function, QMetaObject::Connection{});
        if (!core::contains(ctxActions, action))
            ctxActions.push_back(action);
    }

    void removeContextAction(Action *action) {
        if (actions.contains(action)) {
            QObject::disconnect(actions[action].second);
            actions.erase(action);
            core::erase(ctxActions, action);
            removeObsoleteSeparators();
        }
    }

    void addContextSeparator() {
        if (ctxActions.size() == 0 || ctxActions[ctxActions.size() - 1] == nullptr)
            return;
        ctxActions.push_back(nullptr);
    }

    void removeObsoleteSeparators() {
        bool actionFound = false;
        bool prevWasSep = false;
        size_t idx = 0;
        auto sz = ctxActions.size();
        ctxActions.erase(std::remove_if(ctxActions.begin(),
                                        ctxActions.end(),
                                        [&actionFound, &prevWasSep, &idx, sz](Action *action) {
                                            bool res = false;
                                            if (action != nullptr) {
                                                actionFound = true;
                                                prevWasSep = false;
                                            } else {
                                                if (!actionFound || prevWasSep || idx == sz - 1)
                                                    res = true;
                                                ;
                                                prevWasSep = true;
                                            }
                                            ++idx;
                                            return res;
                                        }),
                         ctxActions.end());
    }
};

TableView::TableView(QString const &name)
    : d(GREU_MKDET()) {
    setObjectName(name);
    d->ctxMenu = new QMenu;
    horizontalHeader()->setObjectName(name + "_header");
    horizontalHeader()->setMinimumSectionSize(20);
    setSortingEnabled(true);
    setSelectionMode(QAbstractItemView::ExtendedSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

void TableView::addContextAction(ContextAction ctxAction) {
    d->addContextAction(ctxAction.action, ctxAction.function);
}

void TableView::addContextAction(Action *action, ContextActionFunc function) {
    d->addContextAction(action, function);
}

void TableView::removeContextAction(Action *action) {
    d->removeContextAction(action);
}

void TableView::addContextSeparator() {
    d->addContextSeparator();
}

void TableView::addContextActionRequest(ContextActionRequestFunc function) {
    d->requests.push_back(function);
}

void TableView::clearContextActionRequests() {
    d->requests.clear();
}

void TableView::noSorting() {
    setSortingEnabled(false);
    horizontalHeader()->setSortIndicatorShown(false);
    verticalHeader()->setSortIndicatorShown(false);
}

void TableView::contextMenuEvent(QContextMenuEvent *event) {
    d->contextMenuEvent(event, this);
}

} // namespace greu::ui
