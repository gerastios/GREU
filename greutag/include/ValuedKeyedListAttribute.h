#pragma once
#include "KeyedListAttribute.h"
#include "PlainAttribute.h"

namespace greu::tag {

template<typename U, typename K, typename V>
class GREU_PUBLIC(TAG) ValuedKeyedListAttribute : public KeyedListAttribute<K, V> {
public:
    using KeyedListAttribute<K, V>::KeyedListAttribute;

    void setUnit(PlainAttribute<U> *value) { unit_ = value; }

    PlainAttribute<U> *unit() const { return unit_; }

    virtual void clear() override {
        KeyedListAttribute<K, V>::clear();
        unit_->clear();
    }

private:
    PlainAttribute<U> *unit_;
};

} // namespace greu::tag
