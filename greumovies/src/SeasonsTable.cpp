#include "SeasonsTable.h"
#include <QImage>
#include "CreditsTable.h"
#include "EmptyImages.h"
#include "EpisodesTable.h"
#include "Season.h"
#include "Series.h"
#include "Strings.h"
#include "datatypes.h"
#include "greucore/include/Uuid.h"
#include "greusql/include/Query.h"
#include "greutmdb/include/SeasonRequester.h"
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
    SERIES_ID = __LINE__ - SLINE,
    TMDB_ID = __LINE__ - SLINE,
    NUMBER = __LINE__ - SLINE,
    NAME = __LINE__ - SLINE,
    OVERVIEW = __LINE__ - SLINE,
    ORIGINAL_NAME = __LINE__ - SLINE,
    AIR_DATE = __LINE__ - SLINE,
    FALLBACK_USED = __LINE__ - SLINE,
    IMAGE = __LINE__ - SLINE,
    IMAGE_TYPE = __LINE__ - SLINE;
// clang-format on

} // namespace

namespace greu::movies {

SeasonsTable::SeasonsTable()
    : Cleanable("seasons",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {"series_id", GREU_MVDB_TYPE_ID},
                 {Strings::fieldTmdbId, GREU_MVDB_TYPE_NUMBER},
                 {"number", GREU_MVDB_TYPE_NUMBER},
                 {Strings::fieldName, GREU_MVDB_TYPE_NAME},
                 {"overview", GREU_MVDB_TYPE_DESCRIPTION},
                 {"original_name", GREU_MVDB_TYPE_NAME},
                 {"air_date", GREU_MVDB_TYPE_DATE},
                 {"fallback_used", GREU_MVDB_TYPE_BOOLEAN},
                 {"image", GREU_MVDB_TYPE_IMAGE},
                 {"image_type", GREU_MVDB_TYPE_TYPE}},
                {{"tmdb_index", {Strings::fieldTmdbId, "number"}},
                 {"series_index", {"series_id"}}}) {
    Table::create();
}

void SeasonsTable::read(Season *no, Record const &r) {
    no->setSeriesId(r[SERIES_ID]);
    no->setTmdbId({r[TMDB_ID].toInt(), r[NUMBER].toInt()});
    no->setNumber(r[NUMBER].toInt());
    no->setName(r[NAME].toString());
    no->setDescription(r[OVERVIEW].toString());
    no->setOriginalName(r[ORIGINAL_NAME].toString());
    no->setDate(r[AIR_DATE].toDate());
    auto ic = ImageContainer{r[IMAGE].toByteArray(), (ImageContainer::Type) r[IMAGE_TYPE].toInt()};
    no->setImageX(
        ImageContainer{r[IMAGE].toByteArray(), (ImageContainer::Type) r[IMAGE_TYPE].toInt()});
    no->setFallbackUsed(r[FALLBACK_USED].toInt());
    auto credits = CreditsTable::instance()->read(*no);
    no->setCast(std::move(credits.castCredits));
    no->setCrew(std::move(credits.crewCredits));
    for (auto &e : EpisodesTable::instance()->idsOfSeason(no->uuid()))
        no->addEpisodeId(e);
}

SeasonsTable *SeasonsTable::instance() {
    static SeasonsTable i;
    return &i;
}

Season *SeasonsTable::readForTmdb(int32_t tmdbId,
                                  int32_t n,
                                  QString const &originalLanguageCode1,
                                  bool update) {
    auto r = selectEqual({tmdbId, n}, {&fields()[TMDB_ID], &fields()[NUMBER]});
    Season *no = nullptr;
    if (r.size() == 1) {
        if (Object::exists(r[0][ID]))
            no = Object::object<Season>(r[0][ID]);
        else
            no = read(r[0][ID]);
    }
    if (no && !update)
        return no;
    auto od = tmdb::SeasonRequester(tmdbId, n, originalLanguageCode1).read();
    auto series = Series::forTmdb(tmdbId);
    if (!no) {
        no = Object::object<Season>(Uuid{});
        no->setTmdbId({od.seriesId, n});
        series->addSeasonId(no->uuid());
        no->setSeriesId(series->uuid());
        no->setNumber(od.number);
    }
    if (no->name().isEmpty())
        no->setName(od.name);
    if ((no->description().isEmpty() || no->fallbackUsed()) && !od.overview.isEmpty())
        no->setDescription(od.overview);
    if (no->originalName().isEmpty())
        no->setOriginalName(od.originalName);
    if (!no->date().isValid())
        no->setDate(od.airDate);
    if (od.poster.isValid())
        no->setImageX(od.poster);
    no->setFallbackUsed(od.fallbackUsed);
    auto credits = CreditsTable::instance()->read(*no);
    no->setCrew(std::move(credits.crewCredits));
    no->setCast(std::move(credits.castCredits));
    no->setNew();
    return no;
}

Season *SeasonsTable::read(core::Uuid const &u) {
    return Object::object<Season>(u);
}

void SeasonsTable::read(Season *no) {
    auto ro = selectSingle({no->uuid()});
    if (!ro)
        return;
    auto r = ro.value();
    read(no, r);
}

void SeasonsTable::save(Season const *o) {
    if (!o->isNew())
        return;
    insertOrUpdate({o->uuid(),
                    o->seriesId(),
                    o->tmdbId().first,
                    o->number(),
                    o->name(),
                    o->description(),
                    o->originalName(),
                    o->date(),
                    (int) o->fallbackUsed(),
                    o->imageX().data(),
                    (int) o->imageX().type()});
    CreditsTable::instance()->save(o->credits(), *o);
}

uint64_t SeasonsTable::clean() {
    auto q = Query::prepareQuery(
        "select distinct id from seasons where not exists (select * from episodes where "
        "season_id = seasons.id);");
    return deleteRecords(fields().at(ID), selectToDelete(q));
}

FoundObjects SeasonsTable::search(SearchOptions const &options) const {
    return simpleSearch<Season>(options, "seasons", Strings::fieldName, Strings::fieldOverview);
}

set<Uuid> SeasonsTable::idsOfSeries(core::Uuid const &u) {
    auto recs = selectEqual({u}, {&fields()[SERIES_ID]});
    set<Uuid> res;
    for (auto &r : recs)
        res.insert(r[ID]);
    return res;
}

} // namespace greu::movies
