#pragma once
#include <QDate>
#include <QWidget>
#include "EditAlerter.h"
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) DateRangeEdit : public QWidget, public EditAlerter {
public:
    DateRangeEdit(QDate const &from = QDate(), QDate const &to = QDate());

    QDate from() const;
    QDate to() const;
    void setFrom(QDate const &);
    void setTo(QDate const &);

    virtual core::EmptySig &editingDone() const override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
