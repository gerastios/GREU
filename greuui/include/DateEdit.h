#pragma once
#include <QDateEdit>
#include "EditAlerter.h"
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) DateEdit : public QDateEdit, public EditAlerter {
public:
    explicit DateEdit(QDate const &value = QDate());

    QDate value() const;
    void setValue(QDate const &);

    virtual core::EmptySig &editingDone() const override;

protected:
    virtual void paintEvent(QPaintEvent *event) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
