#pragma once
#include <QTableView>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class Action;

class GREU_PUBLIC(UI) TableView : public QTableView {
public:
    explicit TableView(QString const &name);

    using ContextActionFunc = std::function<void(int row, int column, QAbstractTableModel *)>;
    struct ContextAction {
        Action *action;
        ContextActionFunc function;
    };
    void addContextAction(ContextAction);
    void addContextAction(Action *, ContextActionFunc);
    void removeContextAction(Action *);
    void addContextSeparator();

    using ContextActionRequestFunc = std::function<std::vector<ContextAction>(int, int)>;
    void addContextActionRequest(ContextActionRequestFunc);
    void clearContextActionRequests();

    void noSorting();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
