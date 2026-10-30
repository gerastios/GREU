#pragma once

#include "AbstractAttributeEditor.h"
#include "greucore/include/DetailPtr.h"

namespace greu::ui {
class EditAlerter;
} // namespace greu::ui

namespace greu::tag {
class AttributeType;
class Attribute;
} // namespace greu::tag

namespace greu::grylizo {

class AttributeEditor : public AbstractAttributeEditor {
public:
    AttributeEditor(tag::AttributeType const *,
                    bool hasTagContainer = true,
                    bool hasTagSelected = true);

    tag::AttributeType const *type() const;
    bool hasTagContainer() const;
    bool hasTagSelected() const;

    tag::Attribute *attribute() const;
    virtual void setValue(tag::Attribute *attribute) = 0;

    virtual void commit() override;

    void undo();
    void tagSelected();
    void tagContainer();

    bool isReadOnly() const;

    static bool askOverwriteTableAttribute(tag::AttributeType const &type);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
