#include "MoviesTable.h"
#include <QImage>
#include "CreditsTable.h"
#include "EmptyImages.h"
#include "GenresTable.h"
#include "Movie.h"
#include "MovieCollection.h"
#include "SearchOptions.h"
#include "Strings.h"
#include "datatypes.h"
#include "greucore/include/Uuid.h"
#include "greusql/include/Query.h"
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
    TAGLINE = __LINE__ - SLINE,
    TITLE = __LINE__ - SLINE,
    VARIANT_TITLE = __LINE__ - SLINE,
    OVERVIEW = __LINE__ - SLINE,
    ORIGINAL_LANGUAGE = __LINE__ - SLINE,
    ORIGINAL_LANGUAGE_CODE = __LINE__ - SLINE,
    ORIGINAL_TITLE = __LINE__ - SLINE,
    RELEASE_DATE = __LINE__ - SLINE,
    FALLBACK_USED = __LINE__ - SLINE,
    IMAGE = __LINE__ - SLINE,
    IMAGE_TYPE = __LINE__ - SLINE;
// clang-format on

} // namespace

namespace greu::movies {

MoviesTable::MoviesTable()
    : Cleanable("movies",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {Strings::fieldTmdbId, GREU_MVDB_TYPE_NUMBER},
                 {"tagline", GREU_MVDB_TYPE_DESCRIPTION},
                 {Strings::fieldTitle, GREU_MVDB_TYPE_TITLE},
                 {"variant_title", GREU_MVDB_TYPE_TITLE},
                 {Strings::fieldOverview, GREU_MVDB_TYPE_DESCRIPTION},
                 {"original_language", GREU_MVDB_TYPE_NAME},
                 {"original_language_code", GREU_MVDB_TYPE_LANGUAGE_CODE},
                 {"original_title", GREU_MVDB_TYPE_TITLE},
                 {"release_date", GREU_MVDB_TYPE_DATE},
                 {"fallback_used", GREU_MVDB_TYPE_BOOLEAN},
                 {"image", GREU_MVDB_TYPE_IMAGE},
                 {"image_type", GREU_MVDB_TYPE_TYPE}}) {
    Table::create();
}

void MoviesTable::read(Movie *no, Record const &r) {
    no->setTmdbId(r[TMDB_ID].toInt());
    no->setTagline(r[TAGLINE].toString());
    no->setName(r[TITLE].toString());
    no->setVariantTitle(r[VARIANT_TITLE].toString());
    no->setDescription(r[OVERVIEW].toString());
    no->setOriginalLanguage(r[ORIGINAL_LANGUAGE].toString());
    no->setOriginalLanguageCode(r[ORIGINAL_LANGUAGE_CODE].toString());
    no->setOriginalTitle(r[ORIGINAL_TITLE].toString());
    no->setDate(r[RELEASE_DATE].toDate());
    no->setFallbackUsed(r[FALLBACK_USED].toInt());
    auto ic = ImageContainer{r[IMAGE].toByteArray(), (ImageContainer::Type) r[IMAGE_TYPE].toInt()};
    no->setImageX(ic);
    auto credits = CreditsTable::instance()->read(*no);
    no->setCast(std::move(credits.castCredits));
    no->setCrew(std::move(credits.crewCredits));
    no->setGenres(GenresTable::instance()->read(no->uuid()));
}

MoviesTable *MoviesTable::instance() {
    static MoviesTable i;
    return &i;
}

Movie *MoviesTable::read(core::Uuid const &u) {
    if (Object::exists(u))
        return Object::object<Movie>(u);
    return Object::object<Movie>(u);
}

void MoviesTable::read(Movie *no) {
    auto ro = selectSingle({no->uuid()});
    if (!ro)
        return;
    auto r = ro.value();
    read(no, r);
}

void MoviesTable::save(Movie const *o) {
    if (!o->isNew())
        return;
    insertOrUpdate({o->uuid(),
                    o->tmdbId(),
                    o->tagline(),
                    o->name(),
                    o->variantTitle(),
                    o->description(),
                    o->originalLanguage(),
                    o->originalLanguageCode(),
                    o->originalTitle(),
                    o->date(),
                    (int) o->fallbackUsed(),
                    o->imageX().data(),
                    (int) o->imageX().type()});
    CreditsTable::instance()->save(o->credits(), *o);
    GenresTable::instance()->save(o->uuid(), o->genres());
}

uint64_t MoviesTable::clean() {
    auto q = Query::prepareQuery(
        "select distinct id from movies where not exists (select * from files where "
        "medium_type = 1 and medium_id = movies.id);");
    return deleteRecords(fields().at(ID), selectToDelete(q));
}

vector<Uuid> MoviesTable::allMovieIds() const {
    auto rr = selectAll();
    vector<Uuid> res;
    for (auto r : rr)
        res.push_back(r[ID]);
    return res;
}

set<Uuid> MoviesTable::idsOfIndependentMovies() const {
    auto q = Query::prepareQuery("select id from movies where not exists (select * from "
                                 "collectable_ids where collectable_id = movies.id) and not exists "
                                 "(select * from moviecoll_ids where movie_id = movies.id);");
    q.execute();
    decltype(idsOfIndependentMovies()) res;
    while (q.next())
        res.insert(q.value(0));
    return res;
}

FoundObjects MoviesTable::search(SearchOptions const &options) const {
    auto where = buildTargetWhere(options, name(), Strings::fieldTitle, Strings::fieldOverview);
    if (options.movieGenres.size() > 0) {
        QString genreWhere = "exists (select * from genres where id = movies.id and genre in (";
        for (int i = 0; auto &g : options.movieGenres) {
            if (i > 0)
                genreWhere += ", ";
            genreWhere += QString("\"%1\"").arg(g);
            ++i;
        }
        genreWhere += "))";
        where += " and " + genreWhere;
    }
    auto qs = QString("select distinct id from movies where %1").arg(where);
    Query q{qs};
    q.bindValue(":string", options.searchString);
    q.execute();
    decltype(search({})) res;
    while (q.next())
        res.insert(Object::object<Movie>(q.value(0)));
    return res;
}

} // namespace greu::movies
