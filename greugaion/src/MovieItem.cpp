#include "MovieItem.h"
#include <QImage>
#include "FeatureModel.h"
#include "MovieWidget.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/CollectionsTable.h"
#include "greumovies/include/File.h"
#include "greumovies/include/FilesTable.h"
#include "greumovies/include/Movie.h"
#include "greumovies/include/MovieCollection.h"
#include "greumovies/include/MovieCollectionsTable.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct MovieItem::Detail {
    struct Model : public FeatureModel {
        Movie const *const movie;

        Model(Movie const *m)
            : movie(m) {}

        virtual QString title() const override { return movie->name(); }

        virtual QString titleLabel() const override { return QObject::tr("Title"); }

        virtual QImage image() const override { return movie->image(); }

        virtual optional<Uuid> linkedUuid() const override { return movie->uuid(); }

        virtual QString description() const override { return movie->description(); }

        virtual QString descriptionLabel() const override { return QObject::tr("Overview"); }

        virtual QString originalTitle() const override { return movie->originalTitle(); }

        virtual QString tagline() const override { return movie->tagline(); }

        virtual File *file() const override {
            auto fo = movie->file();
            if (fo)
                return fo.value();
            return nullptr;
        }

        virtual QDate releaseDate() const override { return movie->date(); }

        virtual set<QString> genres() const override { return movie->genres(); }

        virtual BelongGroups belongsTo() const override {
            BelongGroups res;
            auto mc = MovieCollectionsTable::instance()->collectionsOfMovie(movie->uuid());
            if (!mc.empty()) {
                BelongGroup g;
                g.type = QObject::tr("movie collections");
                for (auto &coll : mc) {
                    auto c = MovieCollection::movieCollection(coll);
                    g.entries.push_back({c->name(), coll});
                }
                res.push_back(g);
            }
            auto co = CollectionsTable::instance()->collectionIdsOfCollectable(
                {movie->uuid(), Collectable::Type::movie});
            if (!co.empty()) {
                BelongGroup g;
                g.type = QObject::tr("collections");
                for (auto &coll : co) {
                    auto c = Collection::collection(coll);
                    g.entries.push_back({c->nameInCollection(), coll});
                }
                res.push_back(g);
            }
            return res;
        }
    };

    Movie const *const movie;
    unique_ptr<MovieWidget> widget;
    unique_ptr<Model> model;

    Detail(Presentation const &p)
        : movie(safeCast<Movie>(MovieObjectItem::object(p))) {}
};

MovieItem::MovieItem(Presentation const &p)
    : MovieObjectItem(p)
    , d(GREU_MKDET(p)) {
    d->model = make_unique<Detail::Model>(d->movie);
}

Movie const *MovieItem::movie() const {
    return d->movie;
}

PresentationWidget *MovieItem::widget() const {
    if(!d->widget)
        d->widget = make_unique<MovieWidget>(this);
    return d->widget.get();
}

FeatureModel *MovieItem::featureModel() const {
    return d->model.get();
}

unique_ptr<FeatureModel> MovieItem::featureModel(Movie *m) {
    return make_unique<Detail::Model>(m);
}

} // namespace greu::gaion
