#include "SeriesRequester.h"
#include <QJsonArray>
#include "ImageReader.h"
#include "TheMovieDatabase.h"
#include "greucore/include/Language.h"
#include "greucore/include/jsonUtils.h"

namespace greu::tmdb {

using namespace core;

struct SeriesRequester::Detail {
    int32_t id;
};

SeriesRequester::SeriesRequester(int32_t id)
    : TmdbRequester()
    , d(GREU_MKDET()) {
    d->id = id;
}

QUrl SeriesRequester::url() const {
    return QUrl(QString("https://api.themoviedb.org/3/tv/%1").arg(QString::number(d->id)));
}

SeriesRequester::Data SeriesRequester::read() {
    auto json = readTmdb();
    Data res{.id = d->id,
             .genres = GREU_TMDB->fillMovieGenres(json.value("genres")),
             .name = valueString(json, "name"),
             .originalName = valueString(json, "original_name"),
             .originalLanguage = TheMovieDatabase::mapLanguage(
                 valueString(json, "original_language")),
             .originalLanguageCode = valueString(json, "original_language"),
             .overview = valueString(json, "overview"),
             .tagline = valueString(json, "tagline"),
             .firstAirDate = valueDate(json, "first_air_date"),
             .lastAirDate = valueDate(json, "last_air_date")};
    auto ifn = valueString(json, "poster_path");
    if (!ifn.isEmpty())
        res.poster = ImageReader(ifn, false).read();
    if ((res.overview.isEmpty() || !res.poster.isValid() || res.tagline.isEmpty())
        && GREU_TMDB->fallbackLanguage() != GREU_TMDB->language()) {
        res.fallbackUsed = true;
        Fallback f(this);
        auto json = readTmdb();
        if (res.overview.isEmpty())
            res.overview = (valueString(json, "overview"));
        if (res.tagline.isEmpty())
            res.tagline = valueString(json, "tagline");
        if (!res.poster.isValid()) {
            auto ifn = valueString(json, "poster_path");
            if (!ifn.isEmpty())
                res.poster = ImageReader(ifn, false).read();
        }
    }
    auto cb = json.value("created_by").toArray();
    for (auto c : cb) {
        auto o = c.toObject();
        res.creators.push_back({valueString(o, "credit_id"), Credit::creators, valueInt(o, "id")});
    }
    return res;
}

QUrl SeriesRequester::creditUrl() const {
    return QUrl(QString("https://api.themoviedb.org/3/tv/%1/credits").arg(d->id));
}

} // namespace greu::tmdb
