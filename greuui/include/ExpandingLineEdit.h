#pragma once
#include <QLineEdit>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) ExpandingLineEdit : public QLineEdit {
public:
    explicit ExpandingLineEdit(QWidget * = nullptr);

    void setWidgetOwnsGeometry(bool);
    bool widgetOwnsGeometry() const;

protected:
    virtual void changeEvent(QEvent *event) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
