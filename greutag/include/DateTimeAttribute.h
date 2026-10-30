#pragma once
#include <QDateTime>
#include "PlainAttribute.h"

namespace greu::tag {

class GREU_PUBLIC(TAG) DateTimeAttribute : public PlainAttribute<QDateTime> {
public:
    enum Precision {
        Year,
        Month,
        Day,
        Hour,
        Minute,
        Second,
    };

    explicit DateTimeAttribute(AttributeType const *, Tag *);

    void setPrecision(Precision);
    Precision precision() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tag
