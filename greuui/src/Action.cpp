#include "Action.h"
#include <QWidget>

using namespace std;

namespace greu::ui {

using namespace core;

struct Action::Detail {
    void init(Action *that,
              QString const &text,
              QKeySequence const &hotkey1,
              QKeySequence const &hotkey2,
              QIcon const &icon) {
        std::vector<QKeySequence> keys;
        QString tt = text;
        bool hasKeys = false;
        if (!hotkey1.isEmpty()) {
            keys.push_back(hotkey1);
            tt += QString(" (%1").arg(hotkey1.toString());
            hasKeys = true;
        }
        if (!hotkey2.isEmpty()) {
            keys.push_back(hotkey2);
            if (hasKeys)
                tt += QString(", %1").arg(hotkey2.toString());
            hasKeys = true;
        }
        if (keys.size() > 0)
            that->setShortcuts(keys);
        if (hasKeys)
            tt += ")";
        that->setIcon(icon);
        tt.replace(tt.indexOf("&"), 1, "");
        that->setToolTip(tt);
    }
};

Action::Action(QString const &text)
    : QAction(text)
    , d(GREU_MKDET()) {
    d->init(this, text, QKeySequence(), QKeySequence(), QIcon());
}

Action::Action(QIcon const &icon)
    : d(GREU_MKDET()) {
    d->init(this, QString{}, QKeySequence(), QKeySequence(), icon);
}

Action::Action(QIcon const &icon, QString const &text)
    : QAction(text)
    , d(GREU_MKDET()) {
    d->init(this, text, QKeySequence(), QKeySequence(), icon);
}

Action::Action(QString const &text, QIcon const &icon)
    : QAction(text)
    , d(GREU_MKDET()) {
    d->init(this, text, QKeySequence(), QKeySequence(), icon);
}

Action::Action(QString const &text, QKeySequence const &hotkey, QIcon const &icon)
    : QAction(text)
    , d(GREU_MKDET()) {
    d->init(this, text, hotkey, QKeySequence(), icon);
}

Action::Action(QString const &text,
               QKeySequence const &hotkey1,
               QKeySequence const &hotkey2,
               QIcon const &icon)
    : QAction(text)
    , d(GREU_MKDET()) {
    d->init(this, text, hotkey1, hotkey2, icon);
}

Action::Action(QString const &text, QKeySequence const &hotkey)
    : QAction(text)
    , d(GREU_MKDET()) {
    d->init(this, text, hotkey, QKeySequence(), QIcon());
}

Action::Action(QString const &text, QKeySequence const &hotkey1, QKeySequence const &hotkey2)
    : QAction(text)
    , d(GREU_MKDET()) {
    d->init(this, text, hotkey1, hotkey2, QIcon());
}

void Action::setShortcuts(vector<QKeySequence> const &shortcuts) {
    QAction::setShortcuts(ranges::to<QList<QKeySequence>>(shortcuts));
}

} // namespace greu::ui
