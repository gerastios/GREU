#pragma once
#include "ComboBox.h"
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) ExpandingComboBox : public ComboBox {
public:
    explicit ExpandingComboBox(QWidget *parent = nullptr);

    void setWidgetOwnsGeometry(bool);
    bool widgetOwnsGeometry() const;

protected:
    virtual void changeEvent(QEvent *event) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
