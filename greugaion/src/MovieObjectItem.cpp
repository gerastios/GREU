#include "MovieObjectItem.h"
#include "CollectionItem.h"
#include "EpisodeItem.h"
#include "MovieCollectionItem.h"
#include "MovieItem.h"
#include "PersonItem.h"
#include "SeasonItem.h"
#include "SeriesItem.h"
#include "greumovies/include/MovieObject.h"
#include "greumovies/include/Object.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct MovieObjectItem::Detail {
    MovieObject const *const object;

    Detail(Presentation const &p)
        : object(safeCast<MovieObject>(Object::existingObject(p.groupTabs.at(0).uuids.at(0)))) {}
};

unique_ptr<MovieObjectItem> MovieObjectItem::create(Uuid const &u) {
    auto object = Object::existingObject(u);
    assert(object);
    auto movieObject = safeCast<MovieObject>(object);
    switch (movieObject->objectType()) {
    case MovieObject::ObjectType::movie: {
        auto res = make_unique<MovieItem>(Presentation{{u}});
        return res;
    } break;
    case MovieObject::ObjectType::movieCollection: {
        auto res = make_unique<MovieCollectionItem>(Presentation{{u}});
        return res;
    } break;
    case MovieObject::ObjectType::collection: {
        auto res = make_unique<CollectionItem>(Presentation{{u}});
        return res;
    } break;
    case MovieObject::ObjectType::episode: {
        auto res = make_unique<EpisodeItem>(Presentation{{u}});
        return res;
    } break;
    case MovieObject::ObjectType::season: {
        auto res = make_unique<SeasonItem>(Presentation{{u}});
        return res;
    } break;
    case MovieObject::ObjectType::series: {
        auto res = make_unique<SeriesItem>(Presentation{{u}});
        return res;
    } break;
    case MovieObject::ObjectType::person: {
        auto res = make_unique<PersonItem>(Presentation{{u}});
        return res;
    } break;
    }
    assert(false);
}

MovieObject *MovieObjectItem::object(core::Uuid const &u) {
    return safeCast<MovieObject>(Object::existingObject(u));
}

MovieObject *MovieObjectItem::object(Presentation const &p) {
    return object(p.groupTabs.at(0).uuids.at(0));
}

MovieObjectItem::MovieObjectItem(Presentation const &p)
    : PresentationItem(p)
    , d(GREU_MKDET(p)) {}

MovieObject const *MovieObjectItem::object() const {
    return d->object;
}

} // namespace greu::gaion
