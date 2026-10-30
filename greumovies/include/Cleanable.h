#pragma once
#include "greucore/include/Uuid.h"
#include "greusql/include/Table.h"

namespace greu::sql {
class Query;
}
namespace greu::movies {

struct GREU_PUBLIC(MOVIES) Cleanable: public sql::Table {
    using sql::Table::Table;

    virtual ~Cleanable() = default;
    virtual uint64_t clean() = 0;

protected:
    core::UuidSet selectToDelete(sql::Query &);
    uint64_t deleteRecords(sql::Field const &, core::UuidSet const &);
};

} // namespace greu::movies
