#pragma once
#include "greusql/include/Table.h"

namespace greu::movies {

class GREU_PUBLIC(MOVIES) GenresDefinitionTable : public sql::Table {
public:
    static GenresDefinitionTable *instance();

    void refresh();

    std::set<QString> allMovieGenres() const;
    std::set<QString> allSeriesGenres() const;

private:
    GenresDefinitionTable();
};

} // namespace greu::movies
