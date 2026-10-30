#pragma once

#include "PlainEditor.h"

namespace greu::grylizo {

class TextAttributeEditor : public PlainEditor {
public:
    struct Adapter;

    TextAttributeEditor(tag::AttributeType const *,
                        Adapter *,
                        bool hasTagContainer = true,
                        bool hasTagSelected = true);

    static Adapter *stringAdapter();
    static Adapter *uintAdapter();

protected:
    virtual QWidget *editor() const override;
    virtual ui::EditAlerter *editAlerter() const override;
    virtual void setEditEnabled(bool) override;
    virtual void setValue(tag::Attribute *) override;
    virtual QLineEdit *lineEdit() const override;
    virtual void setReadOnly(bool) override;
    virtual void setValidator(tag::Attribute const *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
