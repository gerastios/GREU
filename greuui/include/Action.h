#pragma once
#include <QAction>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) Action : public QAction {
public:
    Action(QString const &);
    Action(QIcon const &);
    Action(QIcon const &, QString const &);
    Action(QString const &, QIcon const &);
    Action(QString const &, QKeySequence const &, QIcon const &);
    Action(QString const &, QKeySequence const &, QKeySequence const &, QIcon const &);
    Action(QString const &, QKeySequence const &);
    Action(QString const &, QKeySequence const &, QKeySequence const &);

    void setShortcuts(std::vector<QKeySequence> const &shortcuts);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
