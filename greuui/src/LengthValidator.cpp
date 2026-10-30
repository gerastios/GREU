#include "LengthValidator.h"

namespace greu::ui {

using namespace core;

struct LengthValidator::Detail {
public:
    int lengthMin, lengthMax;

    Detail(int lengthMin, int lengthMax)
        : lengthMin(lengthMin)
        , lengthMax(lengthMax) {}
};

LengthValidator::LengthValidator(int lenghtMin, int lengthMax)
    : d(GREU_MKDET(lenghtMin, lengthMax)) {}

QValidator::State LengthValidator::validate(QString &text, int &) const {
    int l = text.length();
    if (l == 0 || (l >= d->lengthMin && l <= d->lengthMax))
        return QValidator::Acceptable;
    if (l < d->lengthMax)
        return QValidator::Intermediate;
    return QValidator::Invalid;
}

void LengthValidator::setLengthMin(int v) {
    d->lengthMin = v;
}

void LengthValidator::setLengthMax(int v) {
    d->lengthMax = v;
}

} // namespace greu::ui
