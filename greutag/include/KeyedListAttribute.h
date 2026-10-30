#pragma once
#include "Attribute.h"
#include "greucore/include/Selector.h"
#include "greucore/include/stringUtils.h" // IWYU pragma: keep   - needed for references
#include "greucore/include/vectorUtils.h"
#include <functional>
#include <vector>

namespace std {

template<>
struct hash<std::pair<uint, QString>> {
    std::size_t operator()(std::pair<int, QString> const &t) const {
        size_t seed = 0;
        boost::hash_combine(seed, t.first);
        boost::hash_combine(seed, t.second.toStdWString());
        return seed;
    }
};

} // namespace std

#define AT_END -1

namespace greu::tag {

template<typename K, typename T>
class GREU_PUBLIC(TAG) KeyedListAttribute : public Attribute {
public:
    using Attribute::Attribute;

    struct KeyConstraint {
        bool isDescriptionDependent, allowsOnlyOneValue, isDescriptionMandatory;
    };

    struct SingleValue {
        K key;
        QString description;
        T value;
        bool operator==(SingleValue const &other) const {
            return key == other.key && description == other.description && value == other.value;
        }
    };

    using ValueType = typename std::vector<SingleValue>;
    using Iterator = typename ValueType::iterator;
    using ConstIterator = typename ValueType::const_iterator;

    bool hasKey(K const &key) const { return keyConstraints_.contains(key); }

    bool isDescriptionDependent() const { return keyConstraint_.isDescriptionDependent; }

    void setDescriptionDependent(bool value) { keyConstraint_.isDescriptionDependent = value; }

    bool isDescriptionMandatory() const { return keyConstraint_.isDescriptionMandatory; }

    void setDescriptionMandatory(bool value) { keyConstraint_.isDescriptionMandatory = value; }

    bool allowsOnlyOneValue() const { return keyConstraint_.allowsOnlyOneValue; }

    void setAllowsOnlyOneValue(bool value) { keyConstraint_.allowsOnlyOneValue = value; }

    KeyConstraint keyConstraint(K const &key) const {
        try {
            return keyConstraints_.at(key);
        } catch (std::out_of_range &) {
            return keyConstraint_;
        }
    }

    void setDescriptionDependent(K const &key, bool value) {
        if (keyConstraints_.contains(key)) {
            keyConstraints_.at(key).isDescriptionDependent = value;
        }
    }

    void setDescriptionMandatory(K const &key, bool value) {
        if (keyConstraints_.contains(key)) {
            keyConstraints_.at(key).descriptionMandatory = value;
        }
    }

    void setAllowsOnlyOneValue(K const &key, bool value) {
        if (keyConstraints_.contains(key)) {
            keyConstraints_.at(key).allowsOnlyOneValue = value;
        }
    }

    Iterator begin() { return values_.values.begin(); }

    Iterator end() { return values_.values.end(); }

    ConstIterator cbegin() const { return values_.values.cbegin(); }

    ConstIterator cend() const { return values_.values.cend(); }

    SingleValue const at(size_t index) const { return values_.values.at(index); }

    T const first(K const &key) const {
        auto v = values_.keyIndexes.at(key);
        if (v.size() > 0)
            return values_.values[v[0]].value;
        return T{};
    }

    size_t size() const { return values_.values.size(); }

    ErrorType addValue(K const &key, QString const &description, T const &value, bool force) {
        assertDescDependent(key);
        auto rc = addValue_(key, description, value, AT_END);
        if (rc == ErrorType::DuplicateKey && force)
            return modifyValue_(key, description, value, indexOf(key, description));
        return rc;
    }

    ErrorType addValue(K const &key, T const &value, bool force) {
        assertDescIndependent(key);
        auto rc = addValue_(key, QString(), value, AT_END);
        if (rc == ErrorType::DuplicateKey && force)
            return modifyValue_(key, QString(), value, indexOf(key));
        return rc;
    }

    ErrorType removeValue(K const &key, QString const &description, T const &value) {
        assertDescDependent(key);
        return removeValue_(key, description, value);
    }

    ErrorType removeValue(K const &key, T const &value) {
        assertDescIndependent(key);
        return removeValue_(key, QString(), value);
    }

    virtual void clear() override {
        if (values_.values.size() == 0)
            return;
        values_.keyIndexes.clear();
        values_.keyDescriptionIndexes.clear();
        values_.values.clear();
        setModified(true);
    }

    ErrorType modifyValue(K const &key, QString const &description, T const &value, size_t index) {
        assertDescDependent(key);
        return modifyValue_(key, description, value, index);
    }

    ErrorType modifyValue(K const &key, T const &value, size_t index) {
        assertDescIndependent(key);
        return modifyValue_(key, QString(), value, index);
    }

    bool contains(K const &key) const {
        try {
            if (values_.keyIndexes.contains(key))
                return values_.keyIndexes.at(key).size() > 0;
        } catch (std::out_of_range &) {
        }
        return false;
    }

    bool contains(K const &key, QString const &description) const {
        try {
            if (values_.keyDescriptionIndexes.contains({key, description}))
                return values_.keyDescriptionIndexes.at({key, description}).size() > 0;
        } catch (std::out_of_range &) {
        }
        return false;
    }

    bool contains(K const &key, QString const &description, T const &value) const {
        return core::contains(values_.values, {key, description, value});
    }

    void setKeySelector(core::Selector *value) { keySelector_ = value; }

    void setKeySelector(core::Selector *value, bool fillKeys) {
        keySelector_ = value;
        if (fillKeys) {
            for (size_t i = 0; i < keySelector_->size(); i++) {
                auto k = keySelector_->atSortedById(i);
                addKey(k.first, keyConstraint_);
            }
        }
    }

    core::Selector *keySelector() const { return keySelector_; }

    void setValueSelector(core::Selector *value) { valueSelector_ = value; }

    core::Selector *valueSelector() const { return valueSelector_; }

    void setValueValidator(QValidator *value) { valueValidator_ = value; }

    void setKeyValidator(QValidator *value) { keyValidator_ = value; }

    QValidator *valueValidator() const { return valueValidator_; }

    QValidator *keyValidator() const { return keyValidator_; }

    void addKey(K const &key, KeyConstraint keyConstraint) { keyConstraints_[key] = keyConstraint; }

    using OnDuplicateFunc = std::function<void(KeyedListAttribute<K, T> &, SingleValue &)>;

    void setDefaultValue(SingleValue const &value, OnDuplicateFunc onDuplicateFunc) {
        defaultValue_ = value;
        this->onDuplicateFunc = onDuplicateFunc;
    }

    SingleValue defaultValue() {
        ErrorType check;
        auto res = defaultValue_;
        do {
            check = checkNewEntry(res.key, res.description, res.value);
            if (check == ErrorType::DuplicateKey) {
                onDuplicateFunc(*this, res);
            }
        } while (check == ErrorType::DuplicateKey);
        return res;
    }

    int nextDuplicateNumber() {
        ++duplicateNumber;
        return duplicateNumber;
    }

    virtual void originalToCurrent() override { values_ = originalValues_; }

    virtual void currentToOriginal() override { originalValues_ = values_; }

    virtual bool isEmpty() const override { return values_.values.size() == 0; }
    virtual void setGenericValue(std::any const &v) override {
        clear();
        auto values = std::any_cast<ValueType>(v);
        for (auto const &vl : values)
            if (isDescriptionDependent())
                addValue(vl.key, vl.description, vl.value, true);
            else
                addValue(vl.key, vl.value, true);
    }
    virtual std::any genericValue() override { return values_.values; }

private:
    struct Value {
        ValueType values;
        std::unordered_map<K, std::vector<size_t>> keyIndexes;
        std::unordered_map<std::pair<K, QString>, std::vector<size_t>> keyDescriptionIndexes;
    };
    Value values_;
    Value originalValues_;
    SingleValue defaultValue_{K{}, QString{}, T{}};
    KeyConstraint keyConstraint_{.isDescriptionDependent = false,
                                 .allowsOnlyOneValue = false,
                                 .isDescriptionMandatory = false};
    std::unordered_map<K, KeyConstraint> keyConstraints_;
    core::Selector *keySelector_ = nullptr;
    core::Selector *valueSelector_ = nullptr;
    QValidator *valueValidator_ = nullptr;
    QValidator *keyValidator_ = nullptr;
    int duplicateNumber = 0;
    OnDuplicateFunc onDuplicateFunc;

    virtual ErrorType copyValue_(Attribute const &from) override {
        auto f = safeCast<tag::KeyedListAttribute<K, T> const>(&from);
        clear();
        for (auto v = f->cbegin(); v != f->cend(); ++v) {
            if (isDescriptionDependent() || keyConstraint(v->key).isDescriptionDependent) {
                addValue(v->key, v->description, v->value, true);
            } else {
                addValue(v->key, v->value, true);
            }
        }
        return ErrorType::NoError;
    }

    // index = 1 for adding at end
    ErrorType addValue_(K const &key, QString const &description, T const &value, int index) {
        assert(index <= int(values_.values.size()));
        auto check = checkNewEntry(key, description, value);
        if (check != ErrorType::NoError)
            return check;
        size_t idx;
        if (index == -1) {
            idx = values_.values.size();
        } else {
            idx = index;
            changeIndexes(idx, 1);
        }
        addValueIntern(values_, key, description, value, idx);
        if (!core::Modifiable::isTracked())
            addValueIntern(originalValues_, key, description, value, idx);
        setModified(true);
        return ErrorType::NoError;
    }

    ErrorType checkNewEntry(K const &key, QString const &description, T const &value) {
        try {
            auto kd = keyConstraints_.at(key);
            if (kd.allowsOnlyOneValue && contains(key))
                return ErrorType::DuplicateKey;
            if (kd.isDescriptionMandatory && kd.isDescriptionDependent && description.isEmpty())
                return ErrorType::DescriptionEmpty;
        } catch (std::out_of_range &) {
        }
        if (keyConstraint_.allowsOnlyOneValue && contains(key, description))
            return ErrorType::DuplicateKey;
        if (contains(key, description, value))
            return ErrorType::DuplicateKey;
        if (keyConstraint_.isDescriptionMandatory && keyConstraint_.isDescriptionDependent
            && description.isEmpty())
            return ErrorType::DescriptionEmpty;
        return ErrorType::NoError;
    }

    void addValueIntern(
        Value &values, K const &key, QString const &description, T const &value, size_t idx) {
        auto v1 = values.values.begin();
        std::advance(v1, idx);
        values.values.emplace(v1, SingleValue{key, description, value});
        if (!values.keyIndexes.contains(key))
            values.keyIndexes.insert({key, std::vector<size_t>()});
        values.keyIndexes.at(key).push_back(idx);
        if (!values.keyDescriptionIndexes.contains({key, description}))
            values.keyDescriptionIndexes.insert({{key, description}, std::vector<size_t>()});
        values.keyDescriptionIndexes.at({key, description}).push_back(idx);
    }

    ErrorType modifyValue_(K const &key, QString const &description, T const &value, size_t index) {
        auto old = values_.values.at(index);
        if (old.key == key && old.description == description && old.value == value)
            return ErrorType::NoError;
        try {
            auto kd = keyConstraints_.at(key);
            if (old.key != key && kd.allowsOnlyOneValue) {
                if (contains(key))
                    return ErrorType::DuplicateKey;
            }
            if (kd.isDescriptionMandatory && kd.isDescriptionDependent && description.isEmpty())
                return ErrorType::DescriptionEmpty;
        } catch (std::out_of_range &) {
        }
        if (old.key != key || old.description != description)
            if (keyConstraint_.allowsOnlyOneValue && contains(key, description))
                return ErrorType::DuplicateKey;
        if (keyConstraint_.isDescriptionMandatory && keyConstraint_.isDescriptionDependent
            && description.isEmpty())
            return ErrorType::DescriptionEmpty;
        auto res = removeValue_(old.key, old.description, old.value);
        if (res != ErrorType::NoError)
            return res;
        return addValue_(key, description, value, index);
    }

    ErrorType removeValue_(K const &key, QString const &description, T const &value) {
        auto element = std::find_if(values_.values.begin(),
                                    values_.values.end(),
                                    [key, description, value](SingleValue v) {
                                        return v.key == key && v.description == description
                                               && v.value == value;
                                    });
        if (element == values_.values.end())
            return ErrorType::EntryNotFound;
        auto index = std::distance(values_.values.begin(), element);
        core::erase<size_t>(values_.keyIndexes.at(key), index);
        core::erase<size_t>(values_.keyDescriptionIndexes.at({key, description}), index);
        changeIndexes(index, -1);
        values_.values.erase(element);
        setModified(true);
        return ErrorType::NoError;
    }

    void changeIndexes(size_t index, int value) {
        for (auto x = values_.keyIndexes.begin(); x != values_.keyIndexes.end(); x++)
            for (auto i = x->second.begin(); i != x->second.end(); i++)
                if (*i >= index)
                    (*i) += value;
        for (auto x = values_.keyDescriptionIndexes.begin();
             x != values_.keyDescriptionIndexes.end();
             x++)
            for (auto i = x->second.begin(); i != x->second.end(); i++)
                if (*i >= index)
                    (*i) += value;
    }

    void assertDescDependent(K) {
        assert(keyConstraint_.isDescriptionDependent && "Type is description independent");
    }

    void assertDescIndependent(K key) {
        try {
            assert(!keyConstraints_.at(key).isDescriptionDependent
                   && "Type is description dependent");
        } catch (std::out_of_range &) {
            assert(!keyConstraint_.isDescriptionDependent && "Type is description dependent");
        }
    }

    int indexOf(K const &key) {
        try {
            return values_.keyIndexes[key][0];
        } catch (std::out_of_range &) {
            return -1;
        }
    }

    int indexOf(K const &key, QString const &description) {
        try {
            return values_.keyDescriptionIndexes[{key, description}][0];
        } catch (std::out_of_range &) {
            return -1;
        }
    }
};
} // namespace greu::tag

#undef AT_END
