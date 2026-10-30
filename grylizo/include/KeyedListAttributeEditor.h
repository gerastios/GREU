#pragma once

#include "AttributeEditor.h"

namespace greu::tag {
class AttributeType;
}

namespace greu::ui {
class TableView;
}

class QString;

namespace greu::grylizo {

class KeyedListAttributeEditor : public AttributeEditor {
public:
    struct Adapter;
    using AdapterCreator = std::unique_ptr<Adapter> ();

    KeyedListAttributeEditor(tag::AttributeType const *, QString const &name, AdapterCreator *);

    static std::unique_ptr<KeyedListAttributeEditor::Adapter> imagesAdapter();
    static std::unique_ptr<KeyedListAttributeEditor::Adapter> stringBytesAdapter();
    static std::unique_ptr<KeyedListAttributeEditor::Adapter> stringStringAdapter();
    static std::unique_ptr<KeyedListAttributeEditor::Adapter> uintStringAdapter();
    static std::unique_ptr<KeyedListAttributeEditor::Adapter> uintUintAdapter();

    Adapter *adapter() const;

    virtual QWidget *setupUi() override;
    virtual void update() override;
    virtual void commit() override;
    virtual void setValue(tag::Attribute *attribute) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
