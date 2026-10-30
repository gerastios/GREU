#include "AttributeEditor.h"
#include <QCheckBox>
#include <QMessageBox>
#include "Dispatcher.h"
#include "FileEditor.h"
#include "FileTable.h"
#include "MainWindow.h"
#include "Settings.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/File.h"
#include "greutag/include/Tag.h"

namespace greu::grylizo {

using namespace ui;
using namespace core;
using namespace tag;

struct AttributeEditor::Detail {
    AttributeType const *type;
    bool const hasTagContainer, hasTagSelected;

    Detail(AttributeType const *type, bool hasTagContainer, bool hasTagSelected)
        : type(type)
        , hasTagContainer(hasTagContainer)
        , hasTagSelected(hasTagSelected) {}

    Attribute *attribute() const {
        auto f = GREU_M->fileEditor()->file();
        if (!f)
            return nullptr;
        return f->tag()->attribute(type->id());
    }

    void undo(AttributeEditor *that) {
        auto a = attribute();
        if (a) {
            a->undoChanges();
            that->update();
        }
    }

    void tagSelected() {
        auto att = attribute();
        if (!att)
            return;
        GREU_D->tagSelectedFiles(att);
    }

    void tagContainer() {
        auto att = attribute();
        if (!att)
            return;
        GREU_D->tagContainer(att);
    }
};

AttributeEditor::AttributeEditor(AttributeType const *type,
                                 bool hasTagContainer,
                                 bool hasTagSelected)
    : d(GREU_MKDET(type, hasTagContainer, hasTagSelected)) {}

AttributeType const *AttributeEditor::type() const {
    return d->type;
}

bool AttributeEditor::hasTagContainer() const {
    return d->hasTagContainer;
}

bool AttributeEditor::hasTagSelected() const {
    return d->hasTagSelected;
}

Attribute *AttributeEditor::attribute() const {
    return d->attribute();
}

void AttributeEditor::commit() {
    auto att = d->attribute();
    if (att)
        setValue(att);
}

void AttributeEditor::undo() {
    d->undo(this);
}

void AttributeEditor::tagSelected() {
    d->tagSelected();
}

void AttributeEditor::tagContainer() {
    d->tagContainer();
}

bool AttributeEditor::isReadOnly() const {
    return d->type->isReadOnly();
}

bool AttributeEditor::askOverwriteTableAttribute(tag::AttributeType const &type) {
    auto s = Settings::instance()->overwriteTableAttribute();
    if (s.has_value())
        return s.value();
    QMessageBox msg;
    msg.setWindowTitle(QObject::tr("No %1 selected").arg(type.shortLower()));
    msg.setText(QObject::tr("Do you want to overwrite all %2 completely?").arg(type.pluralLower()));
    auto check = new QCheckBox(QObject::tr("Do not ask again"), &msg);
    msg.setCheckBox(check);
    msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
    auto answer = msg.exec();
    bool res = answer == int(QMessageBox::Yes);
    if (check->checkState() == Qt::CheckState::Checked)
        Settings::instance()->setOverwriteTableAttribute(res);
    return res;
}

} // namespace greu::grylizo
