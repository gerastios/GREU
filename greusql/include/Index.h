#pragma once
#include "Field.h"
#include <initializer_list>
#include <vector>

namespace greu::sql {

struct GREU_PUBLIC(SQL) Index {
    using FieldNames = std::vector<QString>;
    using Indexes = std::vector<Index>;

    Index(QString const &name, FieldNames const &fields, bool unique = false)
        : name(name)
        , fields(fields)
        , unique(unique) {}

    Index(QString const &name, std::initializer_list<QString> const &fields, bool unique = false)
        : name(name)
        , fields(fields)
        , unique(unique) {}

    QString const name;
    FieldNames const fields;
    bool unique;
};

} // namespace greu::sql
