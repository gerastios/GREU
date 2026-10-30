#include "MovieCollectionRequester.h"
#include <QJsonArray>
#include "ImageReader.h"
#include "TheMovieDatabase.h"
#include "greucore/include/Language.h"
#include "greucore/include/jsonUtils.h"

using namespace std;

namespace greu::tmdb {

using namespace core;

struct MovieCollectionRequester::Detail {
    int32_t id;
};

MovieCollectionRequester::MovieCollectionRequester(int32_t id)
    : TmdbRequester()
    , d(GREU_MKDET()) {
    d->id = id;
}

QUrl MovieCollectionRequester::url() const {
    return QUrl(QString("https://api.themoviedb.org/3/collection/%1").arg(QString::number(d->id)));
}

MovieCollectionRequester::Data MovieCollectionRequester::read() {
    auto json = readTmdb();
    Data res{.id = d->id,
             .name = valueString(json, "name"),
             .originalLanguage = TheMovieDatabase::mapLanguage(
                 valueString(json, "original_language")),
             .originalLanguageCode = valueString(json, "original_language"),
             .originalName = valueString(json, "original_name"),
             .overview = valueString(json, "overview"),
             .tagline = valueString(json, "tagline")};
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
    auto parts = json.value("parts").toArray();
    set<QDate> rdates;
    for (auto p : parts) {
        res.movieIds.insert(valueInt(p.toObject(), "id"));
        auto rd = valueDate(p.toObject(), "release_date");
        if (!rd.isNull())
            rdates.insert(rd);
    }
    if (rdates.size() > 0)
        res.releaseDate = *rdates.begin();
    return res;
}

} // namespace greu::tmdb
