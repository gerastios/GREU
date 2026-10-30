#include "GenresTable.h"
#include "Strings.h"
#include "greucore/include/Uuid.h"
#include "datatypes.h"

using namespace std;
using namespace greu::core;
using namespace greu::movies;
using namespace greu::sql;

namespace {
// clang-format off
static int constexpr SLINE = __LINE__ + 1,
    ID = __LINE__ - SLINE,
    GENRE = __LINE__ - SLINE;
// clang-format on

} // namespace

namespace greu::movies {

GenresTable::GenresTable()
    : Table("genres",
            {{Strings::fieldId, GREU_MVDB_TYPE_ID, true}, {"genre", GREU_MVDB_TYPE_NAME, true}},
            {{"genre_index", {"genre"}}}) {
    Table::create();
}

GenresTable *GenresTable::instance() {
    static GenresTable i;
    return &i;
}

set<QString> GenresTable::read(Uuid const &u) {
    auto rs = selectEqual({u}, {&fields()[ID]});
    set<QString> res;
    for (auto &r : rs) {
        res.insert(r[GENRE].toString());
    }
    return res;
}

void GenresTable::save(core::Uuid const &u, set<QString> const &genres) {
    Records recs;
    for (auto &g : genres)
        recs.push_back({u, g});
    deleteAndInsert({u}, {&fields()[ID]}, recs);
}

} // namespace greu::movies
