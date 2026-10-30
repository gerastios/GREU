#pragma once
#include "Dialog.h"

class QValidator;

namespace greu::ui {

class GREU_PUBLIC(UI) ComboInput : public Dialog {
public:
    struct ComboValue {
        QString text;
        QVariant data;
    };
    using ComboValues = std::vector<ComboValue>;

    ComboInput(QString const &, ComboValues const &);

    QVariant result() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
