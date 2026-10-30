#pragma once

#include "CompoundAttributeEditor.h"

namespace greu::tag {
class AttributeType;
}

class QString;

namespace greu::grylizo {

class ValuedKeydListAttributeEditor : public CompoundAttributeEditor {
public:
    struct Adapter;

    ValuedKeydListAttributeEditor(tag::AttributeType *typeUnit,
                                  tag::AttributeType *typeValues,
                                  QString const &name,
                                  Adapter *);

    virtual QWidget *setupUi() override;

    static Adapter *uintUintUintAdapter();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
