#pragma once
#include "Attribute.h"
#include "greucore/include/Selector.h"
#include "greucore/include/utils.h" // IWYU pragma: keep   - needed for references

namespace greu::tag {

template<typename T>
class GREU_PUBLIC(TAG) PlainAttribute : public Attribute {
public:
    using Attribute::Attribute;

    using ValueType = T;

    T value() const { return value_; }

    virtual void setValue(T const &value) {
        if (value_ != value) {
            value_ = value;
            if (!core::Modifiable::isTracked())
                originalValue_ = value;
            else
                setModified(true);
        }
    }

    void setSelector(core::Selector *value) { selector_ = value; }

    core::Selector *selector() const { return selector_; }

    virtual void originalToCurrent() override { value_ = originalValue_; }

    virtual void currentToOriginal() override { originalValue_ = value_; }

    virtual void clear() override { setValue({}); }

    virtual bool isEmpty() const override { return value_ == T{}; }
    virtual void setGenericValue(std::any const &v) override { setValue(std::any_cast<T>(v)); }
    virtual std::any genericValue() override { return value_; }

private:
    T value_ = T{};
    T originalValue_ = T{};
    core::Selector *selector_ = nullptr;

    virtual ErrorType copyValue_(Attribute const &other) override {
        auto a = safeCast<PlainAttribute<T> const>(&other);
        this->setValue(a->value());
        return ErrorType::NoError;
    }
};

} // namespace greu::tag
