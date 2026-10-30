#include "MovieCollectionsTable.h"
#include <QImage>
#include "EmptyImages.h"
#include "MovieCollection.h"
#include "Strings.h"
#include "datatypes.h"
#include "greucore/include/Uuid.h"
#include "greusql/include/Query.h"
#include "greutmdb/include/MovieCollectionRequester.h"
#include "searchUtils.h"

using namespace std;
using namespace greu::core;
using namespace greu::movies;
using namespace greu::sql;
using namespace greu::ui;

namespace {
// clang-format off
static int constexpr SLINE = __LINE__ + 1,
    ID = __LINE__ - SLINE,
    TMDB_ID = __LINE__ - SLINE,
    NAME = __LINE__ - SLINE,
    ORIGINAL_NAME = __LINE__ - SLINE,
    OVERVIEW = __LINE__ - SLINE,
    RELEASE_DATE = __LINE__ - SLINE,
    FALLBACK_USED = __LINE__ - SLINE,
    IMAGE = __LINE__ - SLINE,
    IMAGE_TYPE = __LINE__ - SLINE;
// clang-format on

// clang-format off
static int constexpr M_SLINE = __LINE__ + 1,
    M_ID = __LINE__ - M_SLINE,
    M_MOVIE_ID = __LINE__ - M_SLINE;
// clang-format on

struct MovieIdsTable : public Table {
    MovieIdsTable()
        : Table("moviecoll_ids",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true}, {"movie_id", GREU_MVDB_TYPE_ID, true}},
                {{"movie_index", {"movie_id"}}}) {
        create();
    }

    set<Uuid> read(Uuid const &u) const {
        set<Uuid> res;
        auto rs = selectEqual({u}, {&fields()[M_ID]});
        for (auto &r : rs)
            res.insert(r[M_MOVIE_ID]);
        return res;
    }

    void save(MovieCollection const *c) {
        Records recs;
        for (auto &a : c->movieIds())
            recs.push_back({c->uuid(), a});
        deleteAndInsert({c->uuid()}, {&fields()[M_ID]}, recs);
    }

    set<Uuid> collectionsOfMovie(Uuid const &u) const {
        auto recs = selectEqual({u}, {&fields()[M_MOVIE_ID]});
        decltype(collectionsOfMovie({})) res;
        for (auto &r : recs)
            res.insert(r[M_ID]);
        return res;
    }
};

} // namespace

namespace greu::movies {

struct MovieCollectionsTable::Detail {
    MovieIdsTable idsTable;

    void read(MovieCollection *no, Record const &r) {
        no->setTmdbId(r[TMDB_ID].toInt());
        no->setName(r[NAME].toString());
        no->setOriginalName(r[ORIGINAL_NAME].toString());
        no->setDescription(r[OVERVIEW].toString());
        no->setDate(r[RELEASE_DATE].toDate());
        for (auto &mi : idsTable.read(no->uuid()))
            no->addMovieId(mi);
        auto ic = ImageContainer{r[IMAGE].toByteArray(),
                                 (ImageContainer::Type) r[IMAGE_TYPE].toInt()};
        no->setImageX(ic);
        no->setFallbackUsed(r[FALLBACK_USED].toInt());
    }
};

MovieCollectionsTable::MovieCollectionsTable()
    : Cleanable("movie_collections",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {Strings::fieldTmdbId, GREU_MVDB_TYPE_NUMBER},
                 {Strings::fieldName, GREU_MVDB_TYPE_NAME},
                 {"original_name", GREU_MVDB_TYPE_NAME},
                 {Strings::fieldOverview, GREU_MVDB_TYPE_DESCRIPTION},
                 {"release_date", GREU_MVDB_TYPE_DATE},
                 {"fallback_used", GREU_MVDB_TYPE_BOOLEAN},
                 {"image", GREU_MVDB_TYPE_IMAGE},
                 {"image_type", GREU_MVDB_TYPE_TYPE}},
                {{"tmdb_index", {Strings::fieldTmdbId}}})
    , d(GREU_MKDET()) {
    Table::create();
}

MovieCollectionsTable *MovieCollectionsTable::instance() {
    static MovieCollectionsTable i;
    return &i;
}

MovieCollection *MovieCollectionsTable::readForTmdb(int32_t tmdbId, bool update) {
    auto r = selectEqual({tmdbId}, {&fields()[TMDB_ID]});
    MovieCollection *no = nullptr;
    if (r.size() == 1) {
        if (Object::exists(r[0][ID]))
            no = MovieCollection::movieCollection(r[0][ID]);
        else
            no = read(r[0][ID]);
    }
    if (no && !update)
        return no;
    auto od = tmdb::MovieCollectionRequester(tmdbId).read();
    if (!no) {
        no = Object::object<MovieCollection>(Uuid{});
        no->setTmdbId(od.id);
    }
    if (no->name().isEmpty())
        no->setName(od.name);
    if (no->originalName().isEmpty())
        no->setOriginalName(od.originalName);
    if ((no->description().isEmpty() || no->fallbackUsed()) && !od.overview.isEmpty())
        no->setDescription(od.overview);
    if (!no->date().isValid())
        no->setDate(od.releaseDate);
    if (od.poster.isValid())
        no->setImageX(od.poster);
    no->setFallbackUsed(od.fallbackUsed);
    no->setNew();
    return no;
}

MovieCollection *MovieCollectionsTable::read(core::Uuid const &u) {
    if (Object::exists(u))
        return Object::object<MovieCollection>(u);
    return Object::object<MovieCollection>(u);
}

void MovieCollectionsTable::read(MovieCollection *no) {
    auto ro = selectSingle({no->uuid()});
    if (!ro)
        return;
    auto r = ro.value();
    d->read(no, r);
}

void MovieCollectionsTable::save(MovieCollection const *o) {
    if (!o->isNew())
        return;
    insertOrUpdate({o->uuid(),
                    o->tmdbId(),
                    o->name(),
                    o->originalName(),
                    o->description(),
                    o->date(),
                    (int) o->fallbackUsed(),
                    o->imageX().data(),
                    (int) o->imageX().type()});
    d->idsTable.save(o);
}

uint64_t MovieCollectionsTable::clean() {
    auto q = Query::prepareQuery(
        "delete from moviecoll_ids where not exists (select * from movies where "
        "id = moviecoll_ids.movie_id);");
    q.execute();
    auto res = rowCount();
    q = Query::prepareQuery("select distinct id from movie_collections where not exists (select * "
                            "from moviecoll_ids where "
                            "id = movie_collections.id);");
    res += deleteRecords(fields().at(ID), selectToDelete(q));
    return res;
}

FoundObjects MovieCollectionsTable::search(SearchOptions const &options) const {
    return simpleSearch<MovieCollection>(options,
                                         "movie_collections",
                                         Strings::fieldName,
                                         Strings::fieldOverview);
}

vector<Uuid> MovieCollectionsTable::allMovieCollectionIds(bool onlyMoreThaOne) const {
    Records rr;
    if (onlyMoreThaOne) {
        rr = selectFromQuery("select * from movie_collections where (select count(*) from "
                             "moviecoll_ids where id = movie_collections.id) > 1;");
    } else {
        rr = selectAll();
    }
    vector<Uuid> res;
    for (auto r : rr)
        res.push_back(r[ID]);
    return res;
}

set<Uuid> MovieCollectionsTable::collectionsOfMovie(Uuid const &u) const {
    return d->idsTable.collectionsOfMovie(u);
}

} // namespace greu::movies
