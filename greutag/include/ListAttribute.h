#pragma once
#include "Attribute.h"
#include "greucore/include/Selector.h"
#include "greucore/include/vectorUtils.h"
#include <vector>

namespace greu::tag {

template<typename T>
class GREU_PUBLIC(TAG) ListAttribute : public Attribute {
public:
    using ValueType = std::vector<T>;
    using Iterator = typename ValueType::iterator;
    using ConstIterator = typename ValueType::const_iterator;

    using Attribute::Attribute;

    void addValue(T const &value) {
        values_.push_back(value);
        if (!core::Modifiable::isTracked())
            originalValues_.push_back(value);
        else
            setModified(true);
    }

    void insertValue(T const &value, int index) {
        auto it = values_.begin() + index;
        values_.insert(it, value);
        setModified(true);
    }

    void removeValue(int index) {
        auto it = values_.begin() + index;
        if (it < values_.end()) {
            values_.erase(it);
            setModified(true);
        }
    }

    bool contains(T const &value) { return core::contains(values_, value); }

    Iterator begin() { return values_.begin(); }

    Iterator end() { return values_.end(); }

    ConstIterator cbegin() const { return values_.cbegin(); }

    ConstIterator cend() const { return values_.cend(); }

    size_t size() const { return values_.size(); }

    T at(int index) const { return values_.at(index); }

    void setValue(int index, T const &value) {
        if (values_.at(index) != value) {
            values_.at(index) = value;
            setModified(true);
        }
    }

    virtual void clear() override {
        if (size() == 0)
            return;
        values_.clear();
        setModified(true);
    }

    void setSelector(core::Selector *value) { selector_ = value; }

    core::Selector *selector() const { return selector_; }

    virtual void originalToCurrent() override { values_ = originalValues_; }

    virtual void currentToOriginal() override { originalValues_ = values_; }

    virtual bool isEmpty() const override { return values_.size() == 0; }
    virtual void setGenericValue(std::any const &v) override {
        clear();
        for (auto const &v : std::any_cast<ValueType>(v))
            addValue(v);
    }
    virtual std::any genericValue() override { return values_; }

private:
    ValueType values_;
    ValueType originalValues_;
    core::Selector *selector_ = nullptr;
};

} // namespace greu::tag
