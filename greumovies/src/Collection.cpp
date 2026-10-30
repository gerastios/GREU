#include "Collection.h"
#include <QImage>
#include "CollectionsTable.h"
#include "EmptyImages.h"
#include "Episode.h"
#include "Movie.h"
#include "MovieCollection.h"
#include "Object.h"
#include "Season.h"
#include "Series.h"
#include "greucore/include/vectorUtils.h"

using namespace std;
using namespace greu::core;
using namespace greu::ui;

namespace greu::movies {

struct Collection::Detail {
    GREU_P_DET(sortOrder, SortOrder);

    vector<Collectable::Key> collectableKeys;

    void sort() {
        ranges::sort(collectableKeys, [this](Collectable::Key const &f, Collectable::Key const &s) {
            auto cf = Collection::collectable(f);
            auto cs = Collection::collectable(s);
            switch (sortOrder) {
            case byText: {
                return cf->nameInCollection() < cs->nameInCollection();
            } break;
            case byDate:
                return dynamic_cast<MovieObject const *>(cf)->date() < dynamic_cast<MovieObject const *>(cs)->date();
                break;
            }
            return true;
        });
    }
};

Collection::Collection(Uuid const &u)
    : MovieObject(u)
    , d(GREU_MKDET()) {}

QImage Collection::emptyImage() const {
    return EmptyImages::poster();
}

Collection *Collection::collection(Uuid const &u) {
    if (Object::exists(u))
        return Object::object<Collection>(u);
    return CollectionsTable::instance()->read(u);
}

void Collection::read() {
    CollectionsTable::instance()->read(this);
}

void Collection::save() {
    CollectionsTable::instance()->save(this);
}

vector<Collectable::Key> const &Collection::collectableKeys() const {
    return d->collectableKeys;
}

void Collection::addCollectableKey(Key const &k) {
    if (contains(d->collectableKeys, k))
        return;
    d->collectableKeys.push_back(k);
    d->sort();
}

void Collection::removeCollectableKey(Key const &k) {
    if (!contains(d->collectableKeys, k))
        return;
    erase(d->collectableKeys, k);
}

Collectable *Collection::collectable(Collectable::Key const &k) {
    switch (k.type) {
    case Type::movie:
        return movie(k);
        break;
    case Type::series:
        return series(k);
        break;
    case Type::episode:
        return episode(k);
        break;
    case Type::collection:
        return collection(k);
        break;
    case Type::movieCollection:
        return movieCollection(k);
        break;
    }
    return Movie::movie(k.uuid);
}

Movie *Collection::movie(Collectable::Key const &k) {
    if (k.type != Type::movie)
        return nullptr;
    return Movie::movie(k.uuid);
}

Series *Collection::series(Collectable::Key const &k) {
    if (k.type != Type::series)
        return nullptr;
    return Series::series(k.uuid);
}

Episode *Collection::episode(Collectable::Key const &k) {
    if (k.type != Type::episode)
        return nullptr;
    return Episode::episode(k.uuid);
}

Collection *Collection::collection(Collectable::Key const &k) {
    if (k.type != Type::collection)
        return nullptr;
    return Collection::collection(k.uuid);
}

MovieCollection *Collection::movieCollection(Collectable::Key const &k) {
    if (k.type != Type::movieCollection)
        return nullptr;
    return MovieCollection::movieCollection(k.uuid);
}

GREU_P_IMP(Collection, sortOrder, SortOrder, Collection::SortOrder);

} // namespace greu::movies
