#include "Series.h"
#include <QImage>
#include <QString>
#include "EmptyImages.h"
#include "Season.h"
#include "SeriesTable.h"
#include "greutmdb/include/SeriesRequester.h"

using namespace std;
using namespace greu::core;
using namespace greu::ui;
using namespace greu::movies;

namespace greu::movies {

struct Series::Detail {
    GREU_P_DET(tmdbId, int32_t);
    GREU_P_DET(lastAirDate, QDate);
    GREU_P_DET(originalName, QString);
    GREU_P_DET(tagline, QString);
    GREU_P_DET(originalLanguage, QString);
    GREU_P_DET(originalLanguageCode, QString);
    GREU_P_DET(genres, std::set<QString>)
    set<Uuid> seasonIds;
    vector<tmdb::Credit> creatorCredits;
    Credit::Credits credits;
};

Series::Series(Uuid const &u)
    : CreditHolder(u)
    , d(GREU_MKDET()) {}

Series *Series::series(core::Uuid const &u) {
    return SeriesTable::instance()->read(u);
}

Series *Series::forTmdb(int32_t id) {
    return SeriesTable::instance()->readForTmdb(id);
}

void Series::read() {
    SeriesTable::instance()->read(this);
}

void Series::addSeasonId(Uuid const &mi) {
    d->seasonIds.insert(mi);
}

set<Uuid> const &Series::seasonIds() const {
    return d->seasonIds;
}

void Series::setTmdbId(int32_t const &v) {
    d->tmdbId = v;
}

int32_t const &Series::tmdbId() const {
    return d->tmdbId;
}

void Series::save() {
    SeriesTable::instance()->save(this);
}

void Series::update() {
    SeriesTable::instance()->readForTmdb(this->tmdbId(), true);
    save();
}

Series::Credits const *Series::castCredits() const {
    return &d->credits.castCredits;
}

Series::Credits const *Series::crewCredits() const {
    return &d->credits.crewCredits;
}

Series::Credits const *Series::creatorCredits() const {
    return &d->credits.creatorsCredits;
}

Series::Credits const *Series::guestStarsCredits() const {
    return &d->credits.guestStarsCredits;
}

vector<Season *> const Series::seasons() const {
    vector<Season *> res;
    ranges::transform(seasonIds(), back_inserter(res), [](Uuid const &uuid) {
        return Season::season(uuid);
    });
    ranges::sort(res, [](Season const *fst, Season const *snd) {
        return fst->number() < snd->number();
    });
    return res;
}

void Series::setCast(Credits &&v) {
    d->credits.castCredits = std::move(v);
}

void Series::setCrew(Credits &&v) {
    d->credits.crewCredits = std::move(v);
}

void Series::setCreators(Credits &&v) {
    d->credits.creatorsCredits = std::move(v);
}

Credit::Credits const &Series::credits() const {
    return d->credits;
}

QImage Series::emptyImage() const {
    return EmptyImages::poster();
}

std::optional<QUrl> Series::tmdbUrl() const {
    if (d->tmdbId == 0)
        return nullopt;
    return tmdb::SeriesRequester{d->tmdbId}.creditUrl();
}

vector<tmdb::Credit> Series::tmdbCreatorCredits() const {
    return d->creatorCredits;
}

void Series::setCreatorCredits(vector<tmdb::Credit> &&cr) {
    d->creatorCredits = std::move(cr);
}

GREU_P_IMP(Series, lastAirDate, lastAirDate, QDate);
GREU_P_IMP(Series, originalName, OriginalName, QString);
GREU_P_IMP(Series, tagline, Tagline, QString);
GREU_P_IMP(Series, originalLanguage, OriginalLanguage, QString);
GREU_P_IMP(Series, originalLanguageCode, OriginalLanguageCode, QString);
GREU_P_IMP(Series, genres, Genres, std::set<QString>)

} // namespace greu::movies
