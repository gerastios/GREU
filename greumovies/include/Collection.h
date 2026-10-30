#pragma once
#include <QDate>
#include <QImage>
#include "Collectable.h"
#include "MovieObject.h"
#include "greucore/include/DetailPtr.h"
#include "property.h"
#include <vector>

namespace greu::core {
class Uuid;
}

namespace greu::ui {
class ImageContainer;
}

namespace greu::movies {

class Movie;
class Series;
class Episode;
class MovieCollection;

class GREU_PUBLIC(MOVIES) Collection : public MovieObject, public Collectable {
public:
    enum SortOrder : int {
        byText = 0,
        byDate = 1,
    };

    static Collection *collection(core::Uuid const &);

    virtual void read() override;
    virtual void save() override;
    virtual void update() override {};
    virtual Key key() const override { return {uuid(), Collectable::collection}; }

    virtual ObjectType objectType() const override { return ObjectType::collection; }

    std::vector<Collectable::Key> const &collectableKeys() const;
    void addCollectableKey(Key const &);
    void removeCollectableKey(Key const &);

    static Collectable *collectable(Collectable::Key const &);
    static Movie *movie(Collectable::Key const &);
    static Series *series(Collectable::Key const &);
    static Episode *episode(Collectable::Key const &);
    static Collection *collection(Collectable::Key const &);
    static MovieCollection *movieCollection(Collectable::Key const &);

    virtual QString nameInCollection() const override { return MovieObject::name(); }
    GREU_P_DEF(sortOrder, SortOrder, SortOrder);

private:
    friend class Object;
    Collection(core::Uuid const &);
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual QImage emptyImage() const override;

};

} // namespace greu::movies
