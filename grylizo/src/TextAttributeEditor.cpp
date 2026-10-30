#include "TextAttributeEditor.h"
#include "greutag/include/PlainAttribute.h"
#include "greuui/include/LineEdit.h"
#include "include/GuardedPtr.h"

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct TextAttributeEditor::Adapter {
    virtual QString attributeValue(Attribute const *) const = 0;
    virtual void setValueFromEditor(Attribute *, QLineEdit const *) = 0;
    virtual void setValidatorInEditor(Attribute const *, QLineEdit *) = 0;
    virtual ~Adapter() = default;
};

namespace {

template<typename T>
struct Adapter : public virtual TextAttributeEditor::Adapter {
    virtual inline void setValidatorInEditor(Attribute const *attribute,
                                             QLineEdit *editor) override {
        auto a = dynamic_cast<PlainAttribute<T> const *>(attribute);
        if (a)
            editor->setValidator(a->validator());
    }
};

struct StringAdapter : public Adapter<QString> {
    static StringAdapter *instance() {
        static StringAdapter i;
        return &i;
    }

    virtual QString attributeValue(Attribute const *attribute) const override {
        auto a = dynamic_cast<PlainAttribute<QString> const *>(attribute);
        if (a)
            return a->value();
        return QString();
    }

    virtual void setValueFromEditor(Attribute *attribute, QLineEdit const *editor) override {
        auto a = dynamic_cast<PlainAttribute<QString> *>(attribute);
        if (a)
            a->setValue(editor->text());
    }
};

struct UintAdapter : public Adapter<uint> {
    static UintAdapter *instance() {
        static UintAdapter i;
        return &i;
    }

    virtual QString attributeValue(Attribute const *attribute) const override {
        auto a = dynamic_cast<PlainAttribute<uint> const *>(attribute);
        if (a)
            return QString::number(a->value());
        return QString();
    }

    virtual void setValueFromEditor(Attribute *attribute, QLineEdit const *editor) override {
        auto a = dynamic_cast<PlainAttribute<uint> *>(attribute);
        if (a) {
            bool valid;
            auto i = editor->text().toUInt(&valid);
            if (valid)
                a->setValue(i);
        }
    }
};

} // namespace

struct TextAttributeEditor::Detail {
    GuardedPtr<LineEdit> editor;
    Adapter *adapter;

    Detail(Adapter *adapter)
        : adapter(std::move(adapter)) {}

    void setReadOnly(bool value) { editor->setReadOnly(value); }

    void setEditEnabled(bool value, TextAttributeEditor *that) {
        if (value) {
            editor->setEnabled(true);
            auto att = that->attribute();
            if (att) {
                editor->setText(adapter->attributeValue(att));
                that->setValidator(att);
            }
        } else {
            editor->setEnabled(false);
            editor->setText("");
        }
    }

    void setValue(Attribute *attribute) { adapter->setValueFromEditor(attribute, editor.get()); }

    void setValidator(Attribute const *attribute) {
        adapter->setValidatorInEditor(attribute, editor.get());
    }
};

TextAttributeEditor::TextAttributeEditor(AttributeType const *type,
                                         Adapter *adapter,
                                         bool hasTagContainer,
                                         bool hasTagSelected)
    : PlainEditor(type, hasTagContainer, hasTagSelected)
    , d(GREU_MKDET(adapter)) {
    d->editor = new LineEdit;
}

TextAttributeEditor::Adapter *TextAttributeEditor::stringAdapter() {
    return StringAdapter::instance();
}

TextAttributeEditor::Adapter *TextAttributeEditor::uintAdapter() {
    return UintAdapter::instance();
}

QWidget *TextAttributeEditor::editor() const {
    return d->editor.get();
}

EditAlerter *TextAttributeEditor::editAlerter() const {
    return d->editor.get();
}

void TextAttributeEditor::setEditEnabled(bool value) {
    d->setEditEnabled(value, this);
}

void TextAttributeEditor::setValue(Attribute *attribute) {
    d->setValue(attribute);
}

QLineEdit *TextAttributeEditor::lineEdit() const {
    return d->editor.get();
}

void TextAttributeEditor::setReadOnly(bool value) {
    d->setReadOnly(value);
}

void TextAttributeEditor::setValidator(Attribute const *attribute) {
    d->setValidator(attribute);
}

} // namespace greu::grylizo
