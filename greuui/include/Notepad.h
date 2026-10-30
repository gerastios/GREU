#pragma once
#include "Dialog.h"

namespace greu::ui {

class GREU_PUBLIC(UI) Notepad: public Dialog {
public:
    explicit Notepad(QString const & name);

    void setupUi();

    QString text() const;
    void setText(QString const &);

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

}
