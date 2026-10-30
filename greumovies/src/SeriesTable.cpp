#include "SeriesTable.h"
#include <QImage>
#include "CreditsTable.h"
#include "EmptyImages.h"
#include "GenresTable.h"
#include "SearchOptions.h"
#include "SeasonsTable.h"
#include "Series.h"
#include "Strings.h"
#include "datatypes.h"
#include "greucore/include/Uuid.h"
#include "greusql/include/Query.h"
#include "greutmdb/include/SeriesRequester.h"
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
    FIRST_AIR_DATE = __LINE__ - SLINE,
    LAST_AIR_DATE = __LINE__ - SLINE,
    OVERVIEW = __LINE__ - SLINE,
    ORIGINAL_NAME = __LINE__ - SLINE,
    TAGLINE = __LINE__ - SLINE,
    ORIGINAL_LANGUAGE = __LINE__ - SLINE,
    ORIGINAL_LANGUAGE_CODE = __LINE__ - SLINE,
    FALLBACK_USED = __LINE__ - SLINE,
    IMAGE = __LINE__ - SLINE,
    IMAGE_TYPE = __LINE__ - SLINE;
// clang-format on

} // namespace

namespace greu::movies {

SeriesTable::SeriesTable()
    : Cleanable("series",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {Strings::fieldTmdbId, GREU_MVDB_TYPE_NUMBER},
                 {Strings::fieldName, GREU_MVDB_TYPE_NAME},
                 {"first_air_date", GREU_MVDB_TYPE_DATE},
                 {"last_air_date", GREU_MVDB_TYPE_DATE},
                 {Strings::fieldOverview, GREU_MVDB_TYPE_DESCRIPTION},
                 {"original_name", GREU_MVDB_TYPE_NAME},
                 {"tagline", GREU_MVDB_TYPE_DESCRIPTION},
                 {"original_language", GREU_MVDB_TYPE_LANGUAGE},
                 {"original_language_code", GREU_MVDB_TYPE_LANGUAGE_CODE},
                 {"fallback_used", GREU_MVDB_TYPE_BOOLEAN},
                 {"image", GREU_MVDB_TYPE_IMAGE},
                 {"image_type", GREU_MVDB_TYPE_TYPE}},
                {{"tmdb_index", {Strings::fieldTmdbId}}}) {
    Table::create();
}

void SeriesTable::read(Series *no, Record const &r) {
    no->setTmdbId(r[TMDB_ID].toInt());
    no->setName(r[NAME].toString());
    no->setDate(r[FIRST_AIR_DATE].toDate());
    no->setlastAirDate(r[LAST_AIR_DATE].toDate());
    no->setDescription(r[OVERVIEW].toString());
    no->setOriginalName(r[ORIGINAL_NAME].toString());
    no->setTagline(r[TAGLINE].toString());
    no->setOriginalLanguage(r[ORIGINAL_LANGUAGE].toString());
    no->setOriginalLanguageCode(r[ORIGINAL_LANGUAGE_CODE].toString());
    no->setImageX(
        ImageContainer{r[IMAGE].toByteArray(), (ImageContainer::Type) r[IMAGE_TYPE].toInt()});
    no->setFallbackUsed(r[FALLBACK_USED].toInt());
    auto credits = CreditsTable::instance()->read(*no);
    no->setCast(std::move(credits.castCredits));
    no->setCrew(std::move(credits.crewCredits));
    no->setCreators(std::move(credits.creatorsCredits));
    no->setGenres(GenresTable::instance()->read(no->uuid()));
    for (auto &si : SeasonsTable::instance()->idsOfSeries(no->uuid()))
        no->addSeasonId(si);
}

SeriesTable *SeriesTable::instance() {
    static SeriesTable i;
    return &i;
}

Series *SeriesTable::readForTmdb(int32_t tmdbId, bool update) {
    auto r = selectEqual({tmdbId}, {&fields()[TMDB_ID]});
    Series *no = nullptr;
    if (r.size() == 1) {
        if (Object::exists(r[0][ID]))
            no = Series::series(r[0][ID]);
        else
            no = read(r[0][ID]);
    }
    if (no && !update)
        return no;
    auto od = tmdb::SeriesRequester(tmdbId).read();
    if (!no) {
        no = Object::object<Series>(Uuid{});
        no->setTmdbId(od.id);
    }
    if (no->name().isEmpty())
        no->setName(od.name);
    if (!no->date().isValid())
        no->setDate(od.firstAirDate);
    if (!no->lastAirDate().isValid())
        no->setlastAirDate(od.lastAirDate);
    if ((no->description().isEmpty() || no->fallbackUsed()) && !od.overview.isEmpty())
        no->setDescription(od.overview);
    if (no->originalName().isEmpty())
        no->setOriginalName(od.originalName);
    if ((no->tagline().isEmpty() || no->fallbackUsed()) && !od.tagline.isEmpty())
        no->setTagline(od.tagline);
    if (no->originalLanguage().isEmpty())
        no->setOriginalLanguage(od.originalLanguage);
    if (no->originalLanguageCode().isEmpty())
        no->setOriginalLanguageCode(od.originalLanguageCode);
    if (od.poster.isValid())
        no->setImageX(od.poster);
    no->setFallbackUsed(od.fallbackUsed);
    no->setGenres(od.genres);
    no->setCreatorCredits(std::move(od.creators));
    auto credits = CreditsTable::instance()->read(*no);
    no->setCrew(std::move(credits.crewCredits));
    no->setCast(std::move(credits.castCredits));
    no->setCreators(std::move(credits.creatorsCredits));
    no->setNew();
    return no;
}

Series *SeriesTable::read(core::Uuid const &u) {
    if (Object::exists(u))
        return Object::object<Series>(u);
    return Object::object<Series>(u);
}

void SeriesTable::read(Series *no) {
    auto ro = selectSingle({no->uuid()});
    if (!ro)
        return;
    auto r = ro.value();
    read(no, r);
}

void SeriesTable::save(Series const *o) {
    if (!o->isNew())
        return;
    insertOrUpdate({o->uuid(),
                    o->tmdbId(),
                    o->name(),
                    o->date(),
                    o->lastAirDate(),
                    o->description(),
                    o->originalName(),
                    o->tagline(),
                    o->originalLanguage(),
                    o->originalLanguageCode(),
                    (int) o->fallbackUsed(),
                    o->imageX().data(),
                    (int) o->imageX().type()});
    CreditsTable::instance()->save(o->credits(), *o);
    GenresTable::instance()->save(o->uuid(), o->genres());
}

uint64_t SeriesTable::clean() {
    auto q = Query::prepareQuery(
        "select distinct id from series where not exists (select * from seasons where "
        "series_id = series.id);");
    return deleteRecords(fields().at(ID), selectToDelete(q));
}

vector<Uuid> SeriesTable::allSeriesIds() const {
    auto rr = selectAll();
    vector<Uuid> res;
    for (auto r : rr)
        res.push_back(r[ID]);
    return res;
}

FoundObjects SeriesTable::search(SearchOptions const &options) const {
    auto where = buildTargetWhere(options, name(), Strings::fieldName, Strings::fieldOverview);
    if (options.seriesGenres.size() > 0) {
        QString genreWhere = "exists (select * from genres where id = series.id and genre in (";
        for (int i = 0; auto &g : options.movieGenres) {
            if (i > 0)
                genreWhere += ", ";
            genreWhere += QString("\"%1\"").arg(g);
            ++i;
        }
        genreWhere += "))";
        where += " and " + genreWhere;
    }
    auto qs = QString("select distinct id from series where %1").arg(where);
    Query q{qs};
    q.bindValue(":string", options.searchString);
    q.execute();
    decltype(search({})) res;
    while (q.next())
        res.insert(Object::object<Series>(q.value(0)));
    return res;
}

} // namespace greu::movies
