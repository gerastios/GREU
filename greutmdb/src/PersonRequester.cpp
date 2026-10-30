#include "PersonRequester.h"
#include "ImageReader.h"
#include "TheMovieDatabase.h"
#include "greucore/include/Language.h"
#include "greucore/include/jsonUtils.h"

namespace greu::tmdb {

using namespace core;

struct PersonRequester::Detail {
    int32_t id;
};

PersonRequester::PersonRequester(int32_t id)
    : d(GREU_MKDET()) {
    d->id = id;
}

QUrl PersonRequester::url() const {
    return QUrl(QString("https://api.themoviedb.org/3/person/%1").arg(QString::number(d->id)));
}

PersonRequester::Data PersonRequester::read() {
    auto json = readTmdb();
    Data res{.id = d->id,
             .name = valueString(json, "name"),
             .placeOfBirth = valueString(json, "place_of_birth"),
             .knownForDepartment = valueString(json, "known_for_department"),
             .biography = valueString(json, "biography"),
             .alsoKnownAs = valuesString(json, "also_known_as"),
             .birthday = valueDate(json, "birthday"),
             .deathday = valueDate(json, "deathday")};
    auto ifn = valueString(json, "profile_path");
    if (!ifn.isEmpty())
        res.image = ImageReader(ifn, false).read();
    if ((res.biography.isEmpty() || ifn.isEmpty())
        && GREU_TMDB->fallbackLanguage() != GREU_TMDB->language()) {
        res.fallbackUsed = true;
        Fallback f(this);
        auto json = readTmdb();
        res.biography = valueString(json, "biography");
        auto ifn = valueString(json, "profile_path");
        if (!ifn.isEmpty())
            res.image = ImageReader(ifn, false).read();
    }
    return res;
}

} // namespace greu::tmdb
