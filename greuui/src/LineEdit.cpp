#include "LineEdit.h"
#include <QValidator>
#include "Action.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/Icons.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct LineEdit::Detail {
    EmptySig searchRequested, editingDone;
    bool enableValidateColor = true;
    GuardedPtr<Action> actSearch;

    void init(bool searchable, LineEdit *that) {
        ui::connectTextChanged(that, [this, that](QString const &text) {
            if (enableValidateColor && that->validator() && !that->isHidden() && !that->isReadOnly()
                && that->isEnabled()) {
                QString t = text;
                int l = t.length();
                switch (that->validator()->validate(t, l)) {
                case QValidator::Invalid:
                    that->setStyleSheet(
                        "QLineEdit { background: red; selection-background-color: white; }");
                    break;
                case QValidator::Intermediate:
                    that->setStyleSheet(
                        "QLineEdit { background: yellow; selection-background-color: black; }");
                    break;
                case QValidator::Acceptable:
                    that->setStyleSheet("");
                    break;
                }
            }
        });
        if (searchable) {
            actSearch = new Action("", GREU_ICON(search));
            that->addAction(actSearch.get());
            that->addAction(actSearch.get(), LineEdit::TrailingPosition);
            ui::connect(actSearch.get(), [this] { searchRequested(); });
        }
    }
};

LineEdit::LineEdit(QWidget *parent, bool searchable)
    : QLineEdit(parent)
    , d(GREU_MKDET()) {
    d->init(searchable, this);
    ui::connectFinished(this, [this] { d->editingDone(); });
}

LineEdit::LineEdit(bool searchable)
    : d(GREU_MKDET()) {
    d->init(searchable, this);
    ui::connectFinished(this, [this] { d->editingDone(); });
}

EmptySig &LineEdit::searchRequested() const {
    return d->searchRequested;
}

EmptySig &LineEdit::editingDone() const {
    return d->editingDone;
}

void LineEdit::setEnableValidateColor(bool enable) {
    d->enableValidateColor = enable;
}

} // namespace greu::ui
