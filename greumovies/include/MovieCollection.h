#pragma once
#include <QDate>
#include "Collectable.h"
#include "MovieObject.h"
#include "greucore/include/DetailPtr.h"
#include "property.h"
#include <set>

class QDate;
class QImage;

namespace greu::ui {
class ImageContainer;
}

namespace greu::movies {

class Movie;

class GREU_PUBLIC(MOVIES) MovieCollection : public MovieObject, public Collectable {
public:

    static MovieCollection *movieCollection(core::Uuid const &);

    static MovieCollection *forTmdb(int32_t);

    virtual void read() override;
    virtual void save() override;
    virtual void update() override;

    GREU_P_DEF(tmdbId, TmdbId, int32_t)
    GREU_P_DEF(originalName, OriginalName, QString);

    std::set<core::Uuid> const &movieIds() const;

    virtual ObjectType objectType() const override { return ObjectType::movieCollection; }

    virtual QString nameInCollection() const override { return name(); }
    virtual Key key() const override { return {uuid(), Collectable::Type::movieCollection}; }

    std::vector<Movie *> movies() const;

private:
    friend class Object;
    friend class MovieCollectionsTable;
    friend class Movie;
    friend class MoviesTable;
    MovieCollection(core::Uuid const &);
    void addMovieId(core::Uuid const &);
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual QImage emptyImage() const override;

};

} // namespace greu::movies
