#pragma once
#include <QComboBox>
#include "EditAlerter.h"
#include "greucore/include/DetailPtr.h"

namespace greu::core {
class Selector;
}

namespace greu::ui {

class LineEdit;

class GREU_PUBLIC(UI) ComboBox : public QComboBox, public virtual EditAlerter {
public:
    explicit ComboBox(QWidget * = nullptr);

    core::Selector *selector() const;
    void setSelector(core::Selector *const);

    int id() const;
    LineEdit *lineEdit() const;

    virtual core::EmptySig &editingDone() const override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
