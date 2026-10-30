#include "Season.h"
#include <QImage>
#include "EmptyImages.h"
#include "Episode.h"
#include "SeasonsTable.h"
#include "Series.h"
#include "greucore/include/hashFunctions.h" // IWYU pragma: keep
#include "greutmdb/include/SeasonRequester.h"

using namespace std;
using namespace greu::core;
using namespace greu::ui;
using namespace greu::movies;

namespace greu::movies {

struct Season::Detail {
    GREU_P_DET(tmdbId, IntPair);
    GREU_P_DET(originalName, QString);
    set<Uuid> episodeIds;
    Credit::Credits credits;
    Uuid seriesId;
    QString originalLanguageCode1;
};

Season::Season(Uuid const &u)
    : CreditHolder(u)
    , d(GREU_MKDET()) {}

Season *Season::season(core::Uuid const &u) {
    return SeasonsTable::instance()->read(u);
}

Season *Season::forTmdb(int32_t seriesId, int32_t number, QString const &originalLanguageCode1) {
    Season *res;
    res = SeasonsTable::instance()->readForTmdb(seriesId, number, originalLanguageCode1);
    res->d->originalLanguageCode1 = originalLanguageCode1;
    return res;
}

void Season::read() {
    SeasonsTable::instance()->read(this);
}

void Season::addEpisodeId(Uuid const &mi) {
    d->episodeIds.insert(mi);
}

set<Uuid> const &Season::episodeIds() const {
    return d->episodeIds;
}

void Season::setTmdbId(Season::IntPair const &v) {
    d->tmdbId = v;
}

Season::IntPair const &Season::tmdbId() const {
    return d->tmdbId;
}

void Season::save() {
    SeasonsTable::instance()->save(this);
}

void Season::update() {
    SeasonsTable::instance()->readForTmdb(d->tmdbId.first,
                                          d->tmdbId.second,
                                          series()->originalLanguageCode(),
                                          true);
    save();
}

int32_t Season::number() const {
    return d->tmdbId.second;
}

Uuid Season::seriesId() const {
    return d->seriesId;
}

Series *Season::series() const {
    return Series::series(d->seriesId);
}

Season::Credits const *Season::castCredits() const {
    return &d->credits.castCredits;
}

Season::Credits const *Season::crewCredits() const {
    return &d->credits.crewCredits;
}

Season::Credits const *Season::creatorCredits() const {
    return &d->credits.creatorsCredits;
}

Season::Credits const *Season::guestStarsCredits() const {
    return &d->credits.guestStarsCredits;
}

vector<Episode *> Season::episodes() const {
    vector<Episode *> res;
    ranges::transform(episodeIds(), back_inserter(res), [](Uuid const &uuid) {
        return Episode::episode(uuid);
    });
    ranges::sort(res, [](Episode const *fst, Episode const *snd) {
        return fst->number() < snd->number();
    });
    return res;
}

void Season::setCast(Credits &&v) {
    d->credits.castCredits = std::move(v);
}

void Season::setCrew(Credits &&v) {
    d->credits.crewCredits = std::move(v);
}

Credit::Credits const &Season::credits() const {
    return d->credits;
}

void Season::setSeriesId(Uuid const &v) {
    d->seriesId = v;
}

void Season::setNumber(int32_t v) {
    d->tmdbId.second = v;
}

QImage Season::emptyImage() const {
    return EmptyImages::poster();
}

std::optional<QUrl> Season::tmdbUrl() const {
    if (d->tmdbId.first == 0)
        return nullopt;
    return tmdb::SeasonRequester{d->tmdbId.first, d->tmdbId.second, d->originalLanguageCode1}
        .creditUrl();
}

GREU_P_IMP(Season, originalName, OriginalName, QString);

} // namespace greu::movies
