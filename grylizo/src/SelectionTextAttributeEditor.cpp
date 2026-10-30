#include "SelectionTextAttributeEditor.h"
#include "greucore/include/PlainSelector.h"
#include "greucore/include/utils.h"
#include "greutag/include/PlainAttribute.h"
#include "greuui/include/ComboBox.h"
#include "greuui/include/LineEdit.h" // IWYU pragma: keep
#include "include/GuardedPtr.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct SelectionTextAttributeEditor::Adapter {
    virtual QString attributeValue(Attribute const *) const = 0;
    virtual void setValueFromEditor(Attribute *, ComboBox const *) = 0;
    virtual void setValidatorInEditor(Attribute const *, ComboBox *) = 0;
    virtual void setSelectorInEditor(Attribute const *, ComboBox *) = 0;
    virtual ~Adapter() = default;
};

namespace {

template<typename T>
struct Adapter : public SelectionTextAttributeEditor::Adapter {
    virtual void setValidatorInEditor(Attribute const *attribute, ComboBox *editor) override {
        auto a = dynamic_cast<PlainAttribute<T> const *>(attribute);
        if (a)
            editor->setValidator(a->validator());
    }
    virtual void setSelectorInEditor(Attribute const *attribute, ComboBox *editor) override {
        auto a = dynamic_cast<PlainAttribute<T> const *>(attribute);
        if (a) {
            auto seltr = a->selector();
            if (seltr) {
                editor->setSelector(seltr);
            } else {
                editor->setSelector(nullptr);
                editor->setEditable(true);
            }
        }
    }
};

struct StringAdapter : public Adapter<QString> {
    static StringAdapter *instance() {
        static StringAdapter i;
        return &i;
    }

    virtual QString attributeValue(Attribute const *attribute) const override {
        auto a = safeCast<PlainAttribute<QString> const>(attribute);
        return a->value();
        return QString();
    }

    virtual void setValueFromEditor(Attribute *attribute, ComboBox const *editor) override {
        auto a = dynamic_cast<PlainAttribute<QString> *>(attribute);
        if (a) {
            auto text = editor->currentText();
            a->setValue(text);
            auto seltr = dynamic_cast<core::PlainSelector *>(a->selector());
            if (seltr) {
                if (!seltr->contains(text) && seltr->isInsertionAllowed())
                    seltr->insert(text);
            }
        }
    }
};

struct UintAdapter : public Adapter<uint> {
    static UintAdapter *instance() {
        static UintAdapter i;
        return &i;
    }

    virtual QString attributeValue(Attribute const *attribute) const override {
        auto a = dynamic_cast<tag::PlainAttribute<uint> const *>(attribute);
        if (a) {
            auto seltr = dynamic_cast<core::PlainSelector *>(a->selector());
            if (seltr)
                return seltr->value(a->value());
            return QString::number(a->value());
        }
        return QString();
    }

    virtual void setValueFromEditor(Attribute *attribute, ComboBox const *edit) override {
        auto a = dynamic_cast<PlainAttribute<uint> *>(attribute);
        if (a) {
            auto text = edit->currentText();
            auto seltr = dynamic_cast<PlainSelector *>(a->selector());
            if (seltr) {
                auto id = seltr->idOf(text);
                a->setValue(id);
                if (!seltr->contains(id) && seltr->isInsertionAllowed())
                    seltr->insert({id, text});
            }
        }
    }
};

} // namespace

struct SelectionTextAttributeEditor::Detail {
    GuardedPtr<ComboBox> editor;
    Adapter *adapter;

    Detail(Adapter *adapter)
        : adapter(adapter) {}

    void setValidator(Attribute const *attribute) {
        adapter->setValidatorInEditor(attribute, editor.get());
    }

    void setSelector(Attribute const *attribute) {
        adapter->setSelectorInEditor(attribute, editor.get());
    }

    void setEditEnabled(bool value, SelectionTextAttributeEditor *that) {
        if (value) {
            editor->setEnabled(true);
            auto att = that->attribute();
            if (att) {
                that->setValidator(att);
                that->setSelector(att);
                editor->setCurrentText(adapter->attributeValue(att));
            }
        } else {
            editor->setEnabled(false);
            editor->setCurrentText("");
        }
    }

    void setValue(Attribute *attribute) { adapter->setValueFromEditor(attribute, editor.get()); }

    void setReadOnly(bool value) { editor->setDisabled(!value); }
};

SelectionTextAttributeEditor::SelectionTextAttributeEditor(AttributeType *type,
                                                           Adapter *adapter,
                                                           bool hasTagContainer,
                                                           bool hasTagSelected)
    : PlainEditor(type, hasTagContainer, hasTagSelected)
    , d(GREU_MKDET(adapter)) {
    d->editor = new ComboBox;
}

SelectionTextAttributeEditor::Adapter *SelectionTextAttributeEditor::stringAdapter() {
    return StringAdapter::instance();
}

SelectionTextAttributeEditor::Adapter *SelectionTextAttributeEditor::uintAdapter() {
    return UintAdapter::instance();
}

void SelectionTextAttributeEditor::setValidator(Attribute const *attribute) {
    d->setValidator(attribute);
}

void SelectionTextAttributeEditor::setSelector(Attribute const *attribute) {
    d->setSelector(attribute);
}

QWidget *SelectionTextAttributeEditor::editor() const {
    return d->editor.get();
}

EditAlerter *SelectionTextAttributeEditor::editAlerter() const {
    return d->editor.get();
}

void SelectionTextAttributeEditor::setEditEnabled(bool value) {
    return d->setEditEnabled(value, this);
}

void SelectionTextAttributeEditor::setValue(Attribute *attribute) {
    d->setValue(attribute);
}

QLineEdit *SelectionTextAttributeEditor::lineEdit() const {
    return d->editor->lineEdit();
}

void SelectionTextAttributeEditor::setReadOnly(bool value) {
    d->setReadOnly(value);
}

} // namespace greu::grylizo
