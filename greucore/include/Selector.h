#pragma once

#include "DetailPtr.h"
#include "signalUtils.h"
#include <boost/signals2.hpp>

namespace greu::core {

class GREU_PUBLIC(CORE) Selector {
public:
    using SingleValue = std::pair<int, QString>;
    using InitializerList = std::initializer_list<SingleValue> const;
    using AtFunction = SingleValue (Selector::*)(size_t) const;

    explicit Selector();
    explicit Selector(QString const &settingsKey);
    explicit Selector(InitializerList const &);
    explicit Selector(QString const &settingsKey, InitializerList const &);

    void allowUserValues(bool);
    bool isUserValuesAllowed() const;
    void setUserValueId(int);
    int userValueId() const;

    void allowInsertion(bool);
    bool isInsertionAllowed() const;

    void allowDeletion(bool);
    bool isDeletionAllowed() const;

    size_t size() const;

    EmptySig &valuesChanged() const;

    virtual void insert(SingleValue const &);
    virtual void remove(int id);

    virtual QString value(int id) const;
    virtual bool contains(int id) const;
    virtual bool contains(QString const &string) const;
    virtual int idOf(QString const &string) const;

    virtual AtFunction atFunction() const;

    SingleValue atSortedById(size_t index) const;
    SingleValue atSortedByString(size_t index) const;

protected:
    void insertInit(SingleValue value);
    bool isChanged() const;

    struct Detail;
    DetailPtr<Detail> d;
};

} // namespace greu::core
