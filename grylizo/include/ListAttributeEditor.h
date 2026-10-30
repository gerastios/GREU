#pragma once

#include "AttributeEditor.h"

namespace greu::tag {
class AttributeType;
}

class QString;

namespace greu::grylizo {

class ListAttributeEditor : public AttributeEditor {
public:
    struct Adapter;

    ListAttributeEditor(tag::AttributeType const *type, QString const &name, Adapter *adapter);

    GREU_NO_MOVE(ListAttributeEditor)

    static Adapter *stringAdapter();

    Adapter *adapter() const;

    virtual QWidget *setupUi() override;
    virtual void update() override;
    virtual void commit() override;
    virtual void setValue(tag::Attribute *) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
