#pragma once
#include <QValidator>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) LengthValidator : public QValidator {
public:
    LengthValidator(int lenghtMin, int lengthMax);

public:
    virtual State validate(QString &, int &) const override;
    void setLengthMin(int);
    void setLengthMax(int);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
