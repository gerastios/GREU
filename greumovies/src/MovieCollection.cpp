#include "MovieCollection.h"
#include <QDate>
#include <QImage>
#include <QString>
#include "EmptyImages.h"
#include "Movie.h"
#include "MovieCollection.h"
#include "MovieCollectionsTable.h"

using namespace std;
using namespace greu::core;
using namespace greu::ui;
using namespace greu::movies;

namespace greu::movies {

struct MovieCollection::Detail {
    GREU_P_DET(tmdbId, int32_t);
    GREU_P_DET(originalName, QString);
    GREU_P_DET(overview, QString);
    set<Uuid> movieIds;
};

MovieCollection *MovieCollection::movieCollection(core::Uuid const &u) {
    if (Object::exists(u))
        return Object::object<MovieCollection>(u);
    return MovieCollectionsTable::instance()->read(u);
}

MovieCollection *MovieCollection::forTmdb(int32_t id) {
    return MovieCollectionsTable::instance()->readForTmdb(id);
}

void MovieCollection::read() {
    MovieCollectionsTable::instance()->read(this);
}

void MovieCollection::save() {
    MovieCollectionsTable::instance()->save(this);
}

void MovieCollection::update() {
    MovieCollectionsTable::instance()->readForTmdb(d->tmdbId, true);
    save();
}

set<Uuid> const &MovieCollection::movieIds() const {
    return d->movieIds;
}

vector<Movie *> MovieCollection::movies() const {
    auto ids = movieIds();
    vector<Movie *> res;
    for (auto const &uuid : ids)
        res.push_back(Movie::movie(uuid));
    ranges::sort(res, [](auto const &fst, auto const &snd) { return fst->date() < snd->date(); });
    return res;
}

MovieCollection::MovieCollection(Uuid const &u)
    : MovieObject(u)
    , d(GREU_MKDET()) {}

void MovieCollection::setTmdbId(int32_t const &v) {
    d->tmdbId = v;
}

int32_t const &MovieCollection::tmdbId() const {
    return d->tmdbId;
}

void MovieCollection::addMovieId(Uuid const &mi) {
    d->movieIds.insert(mi);
}

QImage MovieCollection::emptyImage() const {
    return EmptyImages::poster();
}

GREU_P_IMP(MovieCollection, originalName, OriginalName, QString);

} // namespace greu::movies
