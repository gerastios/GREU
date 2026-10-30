#include "EpisodesTable.h"
#include <QImage>
#include "CreditsTable.h"
#include "Episode.h"
#include "Season.h"
#include "Series.h"
#include "Strings.h"
#include "datatypes.h"
#include "greucore/include/Uuid.h"
#include "greusql/include/Exception.h"
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
    SEASON_ID = __LINE__ - SLINE,
    TMDB_SERIES_ID = __LINE__ - SLINE,
    SEASON_NUMBER = __LINE__ - SLINE,
    NUMBER = __LINE__ - SLINE,
    VARIANT_SEASON_NUMBER = __LINE__ - SLINE,
    VARIANT_NUMBER = __LINE__ - SLINE,
    NAME = __LINE__ - SLINE,
    OVERVIEW = __LINE__ - SLINE,
    AIR_DATE = __LINE__ - SLINE,
    ORIGINAL_NAME = __LINE__ - SLINE,
    FALLBACK_USED = __LINE__ - SLINE,
    IMAGE = __LINE__ - SLINE,
    IMAGE_TYPE = __LINE__ - SLINE;
// clang-format on

} // namespace

namespace greu::movies {

EpisodesTable *EpisodesTable::instance() {
    static EpisodesTable i;
    return &i;
}

Episode *EpisodesTable::read(core::Uuid const &u) {
    if (Object::exists(u))
        return Object::object<Episode>(u);
    return Object::object<Episode>(u);
}

void EpisodesTable::read(Episode *no) {
    auto ro = selectSingle({no->uuid()});
    if (!ro)
        return;
    auto r = ro.value();
    read(no, r);
}

void EpisodesTable::save(Episode const *o) {
    if (!o->isNew())
        return;
    insertOrUpdate({o->uuid(),
                    o->seasonId(),
                    o->tmdbId().seriesId,
                    o->tmdbId().seasonNumber,
                    o->tmdbId().episodeNumber,
                    o->variantSeasonNumber(),
                    o->variantEpisodeNumber(),
                    o->name(),
                    o->description(),
                    o->date(),
                    o->originalName(),
                    (int) o->fallbackUsed(),
                    o->imageX().data(),
                    (int) o->imageX().type()});
    CreditsTable::instance()->save(o->credits(), *o);
    auto season = Season::season(o->seasonId());
    season->save();
    Series::series(season->seriesId())->save();
}

set<Uuid> EpisodesTable::idsOfSeason(core::Uuid const &u) {
    auto recs = selectEqual({u}, {&fields()[SEASON_ID]});
    set<Uuid> res;
    for (auto &r : recs)
        res.insert(r[ID]);
    return res;
}

uint64_t EpisodesTable::clean() {
    auto q = Query::prepareQuery(
        "select distinct id from episodes where not exists (select * from files where "
        "medium_type = 2 and medium_id = episodes.id);");
    return deleteRecords(fields().at(ID), selectToDelete(q));
}

FoundObjects EpisodesTable::search(SearchOptions const &options) const {
    if (options.seriesGenres.size() == 0)
        return simpleSearch<Episode>(options,
                                     "episodes",
                                     Strings::fieldName,
                                     Strings::fieldOverview);
    auto qs = QString("select distinct episodes.id from episodes join seasons on "
                      "episodes.season_id = seasons.id "
                      "join series on series.id = seasons.series_id join genres on genres.id = "
                      "series.id where %1 and genres.genre in (%2)")
                  .arg(buildTargetWhere(options, name(), Strings::fieldName, Strings::fieldOverview))
                  .arg(ranges::to<QString>(
                      views::join_with(views::transform(options.seriesGenres,
                                                        [](auto const &s) {
                                                            return QString("'%1'").arg(s);
                                                        }),
                                       ',')));
    sql::Query q{qs};
    q.bindValue(":string", options.searchString);
    q.execute();
    FoundObjects res;
    while (q.next())
        res.insert(Object::object<Episode>(q.value(0)));
    return res;
}

EpisodesTable::EpisodesTable()
    : Cleanable("episodes",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {"season_id", GREU_MVDB_TYPE_ID},
                 {"tmdb_series_id", GREU_MVDB_TYPE_NUMBER},
                 {"season_number", GREU_MVDB_TYPE_NUMBER},
                 {"number", GREU_MVDB_TYPE_NUMBER},
                 {"variant_season_number", GREU_MVDB_TYPE_NUMBER},
                 {"variant_number", GREU_MVDB_TYPE_NUMBER},
                 {Strings::fieldName, GREU_MVDB_TYPE_TITLE},
                 {Strings::fieldOverview, GREU_MVDB_TYPE_DESCRIPTION},
                 {"air_date", GREU_MVDB_TYPE_DATE},
                 {"original_name", GREU_MVDB_TYPE_TITLE},
                 {"fallback_used", GREU_MVDB_TYPE_BOOLEAN},
                 {"image", GREU_MVDB_TYPE_IMAGE},
                 {"image_type", GREU_MVDB_TYPE_TYPE}},
                {{"tmdb_index", {"tmdb_series_id", "season_number", "number"}},
                 {"season_index", {"season_id"}}}) {
    Table::create();
}

void EpisodesTable::read(Episode *no, Record const &r) {
    no->setTmdbId({r[TMDB_SERIES_ID].toInt(), r[SEASON_NUMBER].toInt(), r[NUMBER].toInt()});
    no->setVariantSeasonNumber(r[VARIANT_SEASON_NUMBER].toInt());
    no->setVariantEpisodeNumber(r[VARIANT_NUMBER].toInt());
    no->setSeasonId(r[SEASON_ID]);
    no->setName(r[NAME].toString());
    no->setDescription(r[OVERVIEW].toString());
    no->setDate(r[AIR_DATE].toDate());
    no->setOriginalName(r[ORIGINAL_NAME].toString());
    no->setImageX(
        ImageContainer{r[IMAGE].toByteArray(), (ImageContainer::Type) r[IMAGE_TYPE].toInt()});
    no->setFallbackUsed(r[FALLBACK_USED].toInt());
    auto credits = CreditsTable::instance()->read(*no);
    no->setCast(std::move(credits.castCredits));
    no->setCrew(std::move(credits.crewCredits));
    no->setGuestStars(std::move(credits.guestStarsCredits));
}

} // namespace greu::movies
