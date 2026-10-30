#pragma once
#include <QDialog>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) Dialog : public QDialog {
public:
    explicit Dialog(Qt::WindowFlags = Qt::WindowFlags());
    Dialog(QString const &name, Qt::WindowFlags = Qt::WindowFlags());

    virtual bool event(QEvent *event) override;

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
