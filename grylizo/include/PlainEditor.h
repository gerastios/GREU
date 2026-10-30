#pragma once

#include "AttributeEditor.h"

class QLineEdit;
class QGridLayout;

namespace greu::grylizo {

class PlainEditor : public AttributeEditor {
public:
    PlainEditor(tag::AttributeType const *, bool hasTagContainer = true, bool hasTagSelected = true);

    virtual void setupUi(QGridLayout *, int row) override;
    virtual void update() override;
    virtual void commit() override;

protected:
    virtual QLineEdit *lineEdit() const = 0;
    virtual QWidget *editor() const = 0;
    virtual ui::EditAlerter *editAlerter() const = 0;
    virtual void setReadOnly(bool) = 0;
    virtual void setEditEnabled(bool) = 0;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
