#pragma once

#include "DetailPtr.h"
#include "Selector.h"

namespace greu::core {

class GREU_PUBLIC(CORE) PlainSelector : public Selector {
public:
    using InitializerList = std::initializer_list<QString> const;

    PlainSelector();
    ~PlainSelector();
    explicit PlainSelector(QString const &settingsKey);
    explicit PlainSelector(InitializerList const &);
    PlainSelector(InitializerList const &, QString const &settingsKey);
    PlainSelector(InitializerList const &list,
                  QString const &settingsKey,
                  bool userValuesAllowed,
                  bool InsertionAllowed,
                  bool DeletionAllowed);
    PlainSelector(QString const &settingsKey,
                  bool userValuesAllowed,
                  bool InsertionAllowed,
                  bool DeletionAllowed);

    using Selector::insert;
    virtual int insert(QString const &value);

protected:
    void insertInit(QString const &value);

private:
    struct Detail;
    DetailPtr<Detail> d;
};

} // namespace greu::core
