#pragma once
#include "greusql/include/Table.h"

namespace greu::core {
class Uuid;
}

namespace greu::movies {

class GREU_PUBLIC(MOVIES) GenresTable : public sql::Table {
public:
    static GenresTable *instance();

    std::set<QString> read(core::Uuid const &);
    void save(core::Uuid const &, std::set<QString> const &);

private:
    GenresTable();
};

} // namespace greu::movies
