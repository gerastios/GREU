#pragma once
#include <QLineEdit>
#include "EditAlerter.h"
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) LineEdit : public QLineEdit, public EditAlerter {
public:
    explicit LineEdit(QWidget *parent, bool searchable = false);
    LineEdit(bool searchable = false);

    core::EmptySig &searchRequested() const;
    virtual core::EmptySig &editingDone() const override;

protected:
    void setEnableValidateColor(bool enable);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
