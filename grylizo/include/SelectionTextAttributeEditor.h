#pragma once

#include "PlainEditor.h"

namespace greu::grylizo {

class SelectionTextAttributeEditor : public PlainEditor {
public:
    struct Adapter;

    SelectionTextAttributeEditor(tag::AttributeType *,
                                 Adapter *,
                                 bool hasTagContainer = true,
                                 bool hasTagSelected = true);

    static Adapter *stringAdapter();
    static Adapter *uintAdapter();

protected:
    virtual void setValidator(tag::Attribute const *);
    virtual void setSelector(tag::Attribute const *);

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
