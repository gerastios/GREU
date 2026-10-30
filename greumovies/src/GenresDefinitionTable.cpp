#include "GenresDefinitionTable.h"
#include "datatypes.h"
#include "greutmdb/include/TheMovieDatabase.h"

using namespace std;
using namespace greu::core;
using namespace greu::movies;
using namespace greu::sql;
using namespace greu::tmdb;

namespace {
// clang-format off
static int constexpr SLINE = __LINE__ + 1,
    GENRE = __LINE__ - SLINE,
    TYPE = __LINE__ - SLINE;
// clang-format on

#define TYPE_MOVIE 1
#define TYPE_SERIES 2

} // namespace

namespace greu::movies {

GenresDefinitionTable::GenresDefinitionTable()
    : Table("genres_definition",
            {{"genre", GREU_MVDB_TYPE_NAME, true}, {"medium_type", GREU_MVDB_TYPE_NUMBER}},
            {{"type_index", {"medium_type"}}}) {
    Table::create();
}

GenresDefinitionTable *GenresDefinitionTable::instance() {
    static GenresDefinitionTable i;
    return &i;
}

void GenresDefinitionTable::refresh() {
    deleteAll();
    auto mgs = tmdb::TheMovieDatabase::instance()->movieGenres();
    for (auto &g : mgs)
        insertOrUpdate({g, TYPE_MOVIE});
    auto sgs = tmdb::TheMovieDatabase::instance()->seriesGenres();
    for (auto &g : sgs)
        insertOrUpdate({g, TYPE_SERIES});
}

set<QString> GenresDefinitionTable::allMovieGenres() const {
    auto recs = selectEqual({TYPE_MOVIE}, {&fields()[TYPE]});
    decltype(allMovieGenres()) res;
    for (auto &r : recs)
        res.insert(r[GENRE].toString());
    return res;
}

set<QString> GenresDefinitionTable::allSeriesGenres() const {
    auto recs = selectEqual({TYPE_SERIES}, {&fields()[TYPE]});
    decltype(allSeriesGenres()) res;
    for (auto &r : recs)
        res.insert(r[GENRE].toString());
    return res;
}

} // namespace greu::movies
