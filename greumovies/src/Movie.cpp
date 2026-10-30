#include "Movie.h"
#include <QImage>
#include "Credit.h"
#include "CreditsTable.h"
#include "EmptyImages.h"
#include "File.h"
#include "FilesTable.h"
#include "MovieCollection.h"
#include "MovieCollectionsTable.h"
#include "MoviesTable.h"
#include "Person.h"
#include "PersonsTable.h"
#include "greucore/include/Language.h"
#include "greutmdb/include/MovieRequester.h"

using namespace greu::core;
using namespace greu::movies;
using namespace greu::ui;
using namespace std;

namespace greu::movies {

struct Movie::Detail {
    GREU_P_DET(tmdbId, int32_t);
    GREU_P_DET(tagline, QString);
    GREU_P_DET(variantTitle, QString);
    GREU_P_DET(originalLanguage, QString);
    GREU_P_DET(originalLanguageCode, QString);
    GREU_P_DET(originalTitle, QString);
    GREU_P_DET(genres, set<QString>);
    Credit::Credits credits;
    vector<MovieCollection *> collsToSave;

    static Person *fillPerson(OwnPerson const &op) {
        Person *res = nullptr;
        if (op.tmdbId != 0)
            res = PersonsTable::instance()->readForTmdb(op.tmdbId);
        else
            res = Object::object<Person>(Uuid{});
        if (!op.name.isEmpty())
            res->setName(op.name);
        if (!op.biography.isEmpty())
            res->setDescription(op.biography);
        if (!op.birthDate.isValid())
            res->setDate(op.birthDate);
        if (!op.deathDate.isValid())
            res->setDeathday(op.deathDate);
        if (!op.image.isEmpty())
            res->setImageX(op.image);
        return res;
    }
};

Movie *Movie::movie(Uuid const &u) {
    return MoviesTable::instance()->read(u);
}

Movie *Movie::createFromTmdb(int32_t tmdbId, QString const &vt) {
    auto m = object<Movie>(Uuid{});
    auto md = tmdb::MovieRequester{tmdbId}.read();
    m->setTmdbId(md.id);
    if (md.movieCollectionId != 0) {
        auto mc = MovieCollection::forTmdb(md.movieCollectionId);
        mc->addMovieId(m->uuid());
        m->d->collsToSave.push_back(mc);
        mc->setNew();
    }
    m->setTagline(md.tagline);
    m->setName(md.title);
    if (vt.isEmpty())
        m->setVariantTitle(md.title);
    else
        m->setVariantTitle(vt);
    m->setDescription(md.overview);
    m->setOriginalLanguage(md.originalLanguage);
    m->setOriginalLanguageCode(md.originalLanguageCode);
    m->setOriginalTitle(md.originalTitle);
    m->setImageX(md.poster);
    m->setDate(md.releaseDate);
    m->setGenres(md.genres);
    m->setFallbackUsed(md.fallbackUsed);
    m->d->credits = CreditsTable::instance()->read(*m);
    m->setNew();
    return m;
}

Movie *Movie::createFromOwnMovie(OwnMovie const &om) {
    auto m = object<Movie>(Uuid{});
    m->setTagline(om.tagline);
    m->setName(om.title);
    m->setVariantTitle(om.title);
    m->setDescription(om.description);
    m->setOriginalTitle(om.originalTitle);
    m->setImageX(om.image);
    m->setDate(om.releaseDate);
    auto olo = Language::fromCode(om.originalLanguageCode);
    if (olo) {
        m->setOriginalLanguageCode(olo.value()->code1());
        m->setOriginalLanguage(olo.value()->name());
    }
    set<QString> gs;
    for (auto &g : om.genres)
        gs.insert(g);
    m->setGenres(gs);
    for (auto &c : om.cast) {
        auto person = Detail::fillPerson(c.person);
        auto credit = Object::object<Credit>(Uuid{});
        credit->setPersonId(person->uuid());
        credit->setCharacter(c.character);
        credit->setOrder(c.order);
        m->d->credits.castCredits.push_back(credit);
    }
    for (auto &c : om.crew) {
        auto person = Detail::fillPerson(c.person);
        auto credit = Object::object<Credit>(Uuid{});
        credit->setPersonId(person->uuid());
        credit->setJob(c.job);
        m->d->credits.crewCredits.push_back(credit);
    }
    m->setNew();
    return m;
}

void Movie::read() {
    MoviesTable::instance()->read(this);
}

Movie::Movie(Uuid const &u)
    : Video(u)
    , d(GREU_MKDET()) {}

QImage Movie::emptyImage() const {
    return EmptyImages::poster();
}

void Movie::save() {
    MoviesTable::instance()->save(this);
    for (auto &c : d->collsToSave)
        c->save();
    d->collsToSave.clear();
}

Movie::Credits const *Movie::castCredits() const {
    return &d->credits.castCredits;
}

Movie::Credits const *Movie::crewCredits() const {
    return &d->credits.crewCredits;
}

Movie::Credits const *Movie::creatorCredits() const {
    return &d->credits.creatorsCredits;
}

Movie::Credits const *Movie::guestStarsCredits() const {
    return &d->credits.guestStarsCredits;
}

void Movie::setCast(Credits &&v) {
    d->credits.castCredits = std::move(v);
}

void Movie::setCrew(Credits &&v) {
    d->credits.crewCredits = std::move(v);
}

Credit::Credits const &Movie::credits() const {
    return d->credits;
}

std::optional<QUrl> Movie::tmdbUrl() const {
    if (d->tmdbId == 0)
        return nullopt;
    return tmdb::MovieRequester{d->tmdbId}.creditUrl();
}

std::optional<File *> Movie::file() const {
    auto fo = FilesTable::instance()->fileOfVideo(this);
    if (!fo)
        return nullopt;
    return File::file(fo.value());
}

QString const &Movie::name(bool r) const {
    return r ? MovieObject::name() : d->variantTitle;
}

GREU_P_IMP(Movie, tmdbId, TmdbId, int32_t);
GREU_P_IMP(Movie, tagline, Tagline, QString);
GREU_P_IMP(Movie, variantTitle, VariantTitle, QString);
GREU_P_IMP(Movie, originalLanguage, OriginalLanguage, QString);
GREU_P_IMP(Movie, originalLanguageCode, OriginalLanguageCode, QString);
GREU_P_IMP(Movie, originalTitle, OriginalTitle, QString);
GREU_P_IMP(Movie, genres, Genres, std::set<QString>);

} // namespace greu::movies
