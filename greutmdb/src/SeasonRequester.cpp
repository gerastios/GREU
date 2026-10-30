#include "SeasonRequester.h"
#include <QJsonArray>
#include "ImageReader.h"
#include "TheMovieDatabase.h"
#include "greucore/include/Language.h"
#include "greucore/include/jsonUtils.h"

namespace greu::tmdb {

using namespace core;

struct SeasonRequester::Detail {
    int32_t seriesId, number;
    QString originalLanguageCode1;
};

SeasonRequester::SeasonRequester(int32_t sid, int32_t n, QString const &olc)
    : TmdbRequester()
    , d(GREU_MKDET()) {
    d->seriesId = sid;
    d->number = n;
    d->originalLanguageCode1 = olc;
}

QUrl SeasonRequester::url() const {
    return QUrl(QString("https://api.themoviedb.org/3/tv/%1/season/%2")
                    .arg(QString::number(d->seriesId))
                    .arg(QString::number(d->number)));
}

SeasonRequester::Data SeasonRequester::read() {
    auto json = readTmdb();
    Data res{.seriesId = d->seriesId,
             .number = d->number,
             .name = valueString(json, "name"),
             .overview = valueString(json, "overview"),
             .airDate = valueDate(json, "air_date")};
    auto ifn = valueString(json, "poster_path");
    if (!ifn.isEmpty())
        res.poster = ImageReader(ifn, false).read();
    if ((res.overview.isEmpty() || !res.poster.isValid())
        && GREU_TMDB->fallbackLanguage() != GREU_TMDB->language()) {
        res.fallbackUsed = true;
        Fallback f(this);
        auto json = readTmdb();
        if (res.overview.isEmpty())
            res.overview = (valueString(json, "overview"));
        if (!res.poster.isValid()) {
            auto ifn = valueString(json, "poster_path");
            if (!ifn.isEmpty())
                res.poster = ImageReader(ifn, false).read();
        }
    }
    if (GREU_TMDB->language().code1() == d->originalLanguageCode1) {
        res.originalName = res.name;
    } else {
        Fallback f(this, d->originalLanguageCode1);
        auto json = readTmdb();
        res.originalName = valueString(json, "name");
    }
    return res;
}

QUrl SeasonRequester::creditUrl() const {
    return QString("https://api.themoviedb.org/3/tv/%1/season/%2/credits")
        .arg(QString::number(d->seriesId))
        .arg(QString::number(d->number));
}

} // namespace greu::tmdb
