#include "DateTimeAttribute.h"

namespace greu::tag {

using namespace core;

struct DateTimeAttribute::Detail {
    Precision precision = Precision::Second;
};

DateTimeAttribute::DateTimeAttribute(AttributeType const *type, Tag *tag)
    : PlainAttribute<QDateTime>(type, tag)
    , d(GREU_MKDET()) {}

void DateTimeAttribute::setPrecision(Precision value) {
    d->precision = value;
}

DateTimeAttribute::Precision DateTimeAttribute::precision() const {
    return d->precision;
}

} // namespace greu::tag
