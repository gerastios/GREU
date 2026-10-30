#pragma once

#include "PlainEditor.h"

namespace greu::grylizo {

class DateTimeAttributeEditor : public PlainEditor {
public:
    DateTimeAttributeEditor(tag::AttributeType *,
                            bool hasTagContainer = true,
                            bool hasTagSelected = true);

protected:
    virtual QWidget *editor() const override;
    virtual ui::EditAlerter *editAlerter() const override;
    virtual void setEditEnabled(bool) override;
    virtual void setValue(tag::Attribute *) override;

    virtual QLineEdit *lineEdit() const override;
    virtual void setReadOnly(bool) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
