#include "EpisodeRequester.h"
#include <QJsonArray>
#include "ImageReader.h"
#include "TheMovieDatabase.h"
#include "greucore/include/Language.h"
#include "greucore/include/jsonUtils.h"

namespace greu::tmdb {

using namespace core;

struct EpisodeRequester::Detail {
    int32_t seriesId, seasonNUmber, number;
    QString originalLanguageCode1;
};

EpisodeRequester::EpisodeRequester(int32_t sid, int32_t sn, int32_t n, QString const &olc)
    : TmdbRequester()
    , d(GREU_MKDET()) {
    d->seriesId = sid;
    d->seasonNUmber = sn;
    d->number = n;
    d->originalLanguageCode1 = olc;
}

QUrl EpisodeRequester::url() const {
    return QUrl(QString("https://api.themoviedb.org/3/tv/%1/season/%2/episode/%3")
                    .arg(QString::number(d->seriesId))
                    .arg(QString::number(d->seasonNUmber))
                    .arg(QString::number(d->number)));
}

EpisodeRequester::Data EpisodeRequester::read() {
    auto json = readTmdb();
    Data res{.seriesId = d->seriesId,
             .seasonNumber = d->seasonNUmber,
             .number = d->number,
             .name = valueString(json, "name"),
             .overview = valueString(json, "overview"),
             .airDate = valueDate(json, "air_date")};
    auto ifn = valueString(json, "still_path");
    if (!ifn.isEmpty())
        res.stillImage = ImageReader(ifn, false).read();
    if (res.overview.isEmpty()) {
        if (GREU_TMDB->fallbackLanguage() != GREU_TMDB->language()) {
            res.fallbackUsed = true;
            Fallback f(this);
            auto json = readTmdb();
            if (res.overview.isEmpty())
                res.overview = (valueString(json, "overview"));
        }
    }
    if (GREU_TMDB->language().code1() == d->originalLanguageCode1) {
        res.originalName = res.name;
    } else {
        if (GREU_TMDB->language().code1().toLower() != d->originalLanguageCode1.toLower()) {
            Fallback f(this, d->originalLanguageCode1);
            auto json = readTmdb();
            res.originalName = valueString(json, "name");
        }
    }
    return res;
}

QUrl EpisodeRequester::creditUrl() const {
    return QString("https://api.themoviedb.org/3/tv/%1/season/%2/episode/%3/credits")
        .arg(QString::number(d->seriesId))
        .arg(QString::number(d->seasonNUmber))
        .arg(QString::number(d->number));
}

} // namespace greu::tmdb
