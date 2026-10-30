#include "PlainEditor.h"
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include "Dispatcher.h"
#include "FileEditor.h"
#include "FileTable.h"
#include "MainWindow.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/utils.h" // IWYU pragma: keep
#include "greutag/include/AttributeType.h"
#include "greutag/include/FileContainer.h"
#include "greuui/include/Action.h"
#include "greuui/include/EditAlerter.h"
#include "greuui/include/Icons.h"
#include "greuui/include/utils.h"
#include "greuui/include/connect.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

namespace {
static GuardedPtr<Action> addAction(QIcon const &icon,
                                    QString const &tooltip,
                                    QLineEdit *lineEdit,
                                    function<void()> triggerFunc) {
    GuardedPtr<Action> result = new Action(icon);
    result->setToolTip(tooltip);
    ui::connect(result.get(), triggerFunc);
    lineEdit->addAction(result.get(), QLineEdit::ActionPosition::TrailingPosition);
    return result;
}
} // namespace

struct PlainEditor::Detail {
    ScopedConnection connModified;
    GuardedPtr<QLabel> label;
    GuardedPtr<Action> actTagContainer, actTagSelected, actUndo;

    void setupUi(QGridLayout *layout, int row, PlainEditor *that) {
        addNewWidget(label, determineLabelText(that), layout, row, 0);
        auto editor = that->editor();
        auto lineEdit = that->lineEdit();
        layout->addWidget(editor, row, 1);
        label->setBuddy(editor);
        if (!that->isReadOnly()) {
            actUndo = addAction(GREU_ICON(undo), QObject::tr("Undo changes"), lineEdit, [that] {
                that->commit();
                that->undo();
            });
        }
        if (lineEdit && that->hasTagSelected()) {
            actTagSelected = addAction(GREU_ICON(tag_selected),
                                       QObject::tr("Tag selected files with this %1")
                                           .arg(that->type()->shortLower()),
                                       lineEdit,
                                       [that] {
                                           that->commit();
                                           that->tagSelected();
                                       });
            actTagSelected->setEnabled(!that->isReadOnly());
        }
        if (lineEdit && that->hasTagContainer()) {
            actTagContainer = addAction(GREU_ICON(tag_container), "", lineEdit, [that] {
                that->commit();
                that->tagContainer();
            });
            actTagSelected->setEnabled(!that->isReadOnly());
        }
        update(that);
    }

    void update(PlainEditor *that) { update(that->attribute(), that); }

    void update(Attribute *attribute, PlainEditor *that) {
        connModified.disconnect();
        if (attribute) {
            connModified = that->editAlerter()->editingDone().connect(0, [that] { that->commit(); });
            label->setEnabled(true);
            that->setEditEnabled(true);
            that->setReadOnly(that->isReadOnly());
            if (actTagContainer) {
                auto container = GREU_D->fileContainer();
                if (container) {
                    actTagContainer->setIcon(container->tagIcon());
                    actTagContainer->setToolTip(
                        QObject::tr("Tag files in %2 with this %1")
                            .arg(that->type()->shortLower(), container->tagText()));
                    actTagContainer->setEnabled(!that->isReadOnly());
                } else {
                    actTagContainer->setEnabled(false);
                }
            }
            if (actTagSelected && !that->isReadOnly())
                actTagSelected->setEnabled(true);
            if (actUndo && !that->isReadOnly())
                actUndo->setEnabled(true);
        } else {
            label->setEnabled(false);
            that->setEditEnabled(false);
            if (actTagContainer)
                actTagContainer->setEnabled(false);
            if (actTagSelected)
                actTagSelected->setEnabled(false);
            if (actUndo && !that->isReadOnly())
                actUndo->setEnabled(false);
        }
    }

    QString determineLabelText(PlainEditor *that) {
        auto text = that->type()->longUpper();
        auto fileEditor = GREU_M->fileEditor();
        int ofs = 0;
        QString result;
        do {
            if (ofs >= text.length()) {
                result = text;
            } else {
                auto c = text.at(ofs);
                if ((c.isDigit() || c.isLetter()) && fileEditor->addMnemonic(c)) {
                    result = text;
                    result.insert(ofs, '&');
                }
            }
            ofs++;
        } while (result.isEmpty());
        return result;
    }
};

PlainEditor::PlainEditor(AttributeType const *type, bool hasTagContainer, bool hasTagSelected)
    : AttributeEditor(type, hasTagContainer, hasTagSelected)
    , d(GREU_MKDET()) {}

void PlainEditor::setupUi(QGridLayout *layout, int row) {
    d->setupUi(layout, row, this);
}

void PlainEditor::update() {
    d->update(this);
}

void PlainEditor::commit() {
    auto att = attribute();
    if (att)
        setValue(att);
}

} // namespace greu::grylizo
