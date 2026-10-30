#include "Episode.h"
#include <QImage>
#include "Credit.h"
#include "CreditsTable.h"
#include "EmptyImages.h"
#include "EpisodesTable.h"
#include "File.h"
#include "FilesTable.h"
#include "Season.h"
#include "Series.h"
#include "greutmdb/include/EpisodeRequester.h"

using namespace greu::core;
using namespace greu::movies;
using namespace greu::ui;
using namespace std;

namespace greu::movies {

struct Episode::Detail {
    GREU_P_DET(tmdbId, TmdbKey);
    GREU_P_DET(variantSeasonNumber, int32_t);
    GREU_P_DET(variantEpisodeNumber, int32_t);
    GREU_P_DET(originalName, QString);
    Credit::Credits credits;
    core::Uuid seasonId;
};

Episode *Episode::episode(Uuid const &u) {
    return EpisodesTable::instance()->read(u);
}

Episode *Episode::createFromTmdb(TmdbKey const &tmdbKey,
                                 int32_t variantSeasonNumber,
                                 int32_t variantEpisodeNumber,
                                 QString const &variantTitle) {
    auto series = Series::forTmdb(tmdbKey.seriesId);
    auto m = object<Episode>(Uuid{});
    auto md = tmdb::EpisodeRequester{tmdbKey.seriesId,
                                     tmdbKey.seasonNumber,
                                     tmdbKey.episodeNumber,
                                     series->originalLanguageCode()}
                  .read();
    m->setTmdbId(tmdbKey);
    Season *season;
    if (variantSeasonNumber == 0)
        season = Season::forTmdb(tmdbKey.seriesId,
                                 tmdbKey.seasonNumber,
                                 series->originalLanguageCode());
    else
        season = Season::forTmdb(tmdbKey.seriesId,
                                 variantSeasonNumber,
                                 series->originalLanguageCode());
    season->addEpisodeId(m->uuid());
    m->setSeasonId(season->uuid());
    if (variantSeasonNumber > 0)
        m->setVariantSeasonNumber(variantSeasonNumber);
    else
        m->setVariantSeasonNumber(tmdbKey.seasonNumber);
    if (variantEpisodeNumber > 0)
        m->setVariantEpisodeNumber(variantEpisodeNumber);
    else
        m->setVariantEpisodeNumber(tmdbKey.episodeNumber);
    if (variantTitle.isEmpty())
        m->setName(md.name);
    else
        m->setName(variantTitle);
    m->setDescription(md.overview);
    m->setDate(md.airDate);
    m->setOriginalName(md.originalName);
    m->setImageX(md.stillImage);
    m->setFallbackUsed(md.fallbackUsed);
    m->d->credits = CreditsTable::instance()->read(*m);
    m->setNew();
    return m;
}

void Episode::read() {
    EpisodesTable::instance()->read(this);
}

Episode::Episode(Uuid const &u)
    : Video(u)
    , d(GREU_MKDET()) {}

QImage Episode::emptyImage() const {
    return EmptyImages::stillImage();
}

void Episode::save() {
    EpisodesTable::instance()->save(this);
}

Episode::Credits const *Episode::creatorCredits() const {
    return &d->credits.creatorsCredits;
}

Episode::Credits const *Episode::castCredits() const {
    return &d->credits.castCredits;
}

Episode::Credits const *Episode::crewCredits() const {
    return &d->credits.crewCredits;
}

Episode::Credits const *Episode::guestStarsCredits() const {
    return &d->credits.guestStarsCredits;
}

void Episode::setCast(Credits &&v) {
    d->credits.castCredits = std::move(v);
}

void Episode::setCrew(Credits &&v) {
    d->credits.crewCredits = std::move(v);
}

void Episode::setGuestStars(Credits &&v) {
    d->credits.guestStarsCredits = std::move(v);
}

Credit::Credits const &Episode::credits() const {
    return d->credits;
}

void Episode::setSeasonId(Uuid const &u) {
    d->seasonId = u;
}

Uuid Episode::seasonId() const {
    return d->seasonId;
}

Season *Episode::season() const {
    return Season::season(d->seasonId);
}

Series *Episode::series() const {
    return season()->series();
}

QString Episode::nameInCollection() const {
    return QString("%1 (%2/%3) - %4")
        .arg(series()->name())
        .arg(seasonNumber())
        .arg(number())
        .arg(name());
}

std::optional<File *> Episode::file() const {
    auto fo = FilesTable::instance()->fileOfVideo(this);
    if (!fo)
        return nullopt;
    return File::file(fo.value());
}

std::optional<QUrl> Episode::tmdbUrl() const {
    if (d->tmdbId.seriesId == 0)
        return nullopt;
    return tmdb::EpisodeRequester{d->tmdbId.seriesId,
                                  d->tmdbId.seasonNumber,
                                  d->tmdbId.episodeNumber,
                                  Series::series(Season::season(d->seasonId)->seriesId())
                                      ->originalLanguageCode()}
        .creditUrl();
}

int32_t Episode::number(bool r) const {
    return r ? d->tmdbId.episodeNumber : d->variantEpisodeNumber;
}

int32_t Episode::seasonNumber(bool r) const {
    return r ? d->tmdbId.seasonNumber : d->variantSeasonNumber;
}

GREU_P_IMP(Episode, tmdbId, TmdbId, Episode::TmdbKey);
GREU_P_IMP(Episode, variantSeasonNumber, VariantSeasonNumber, int32_t);
GREU_P_IMP(Episode, variantEpisodeNumber, VariantEpisodeNumber, int32_t);
GREU_P_IMP(Episode, originalName, OriginalName, QString);

} // namespace greu::movies
