#include "MovieRequester.h"
#include "ImageReader.h"
#include "TheMovieDatabase.h"
#include "greucore/include/Language.h"
#include "greucore/include/jsonUtils.h"

namespace greu::tmdb {

using namespace core;

struct MovieRequester::Detail {
    int32_t id;
};

MovieRequester::MovieRequester(int32_t id)
    : TmdbRequester()
    , d(GREU_MKDET()) {
    d->id = id;
}

QUrl MovieRequester::url() const {
    return QUrl(QString("https://api.themoviedb.org/3/movie/%1").arg(QString::number(d->id)));
}

MovieRequester::Data MovieRequester::read() {
    auto json = readTmdb();
    Data res{.id = d->id,
             .movieCollectionId = valueInt(json.value("belongs_to_collection").toObject(), "id"),
             .genres = GREU_TMDB->fillMovieGenres(json.value("genres")),
             .title = valueString(json, "title"),
             .originalLanguage = TheMovieDatabase::mapLanguage(
                 valueString(json, "original_language")),
             .originalLanguageCode = valueString(json, "original_language"),
             .originalTitle = valueString(json, "original_title"),
             .overview = valueString(json, "overview"),
             .tagline = valueString(json, "tagline"),
             .releaseDate = valueDate(json, "release_date")};
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
    return res;
}

QUrl MovieRequester::creditUrl() const {
    return QUrl(QString("https://api.themoviedb.org/3/movie/%1/credits").arg(d->id));
}

} // namespace greu::tmdb
