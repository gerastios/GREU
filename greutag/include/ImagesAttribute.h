#pragma once
#include <QImage>
#include "KeyedListAttribute.h"

namespace greu::tag {

class GREU_PUBLIC(TAG) ImagesAttribute : public KeyedListAttribute<uint, QImage> {
public:

    using KeyedListAttribute<uint, QImage>::KeyedListAttribute;
    virtual QImage const mainValue() const = 0;
};

} // namespace greu::tag
