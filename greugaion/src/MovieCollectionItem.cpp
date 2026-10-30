#include "MovieCollectionItem.h"
#include <QImage>
#include "FeatureModel.h"
#include "MovieCollectionWidget.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/CollectionsTable.h"
#include "greumovies/include/MovieCollection.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct MovieCollectionItem::Detail {
    struct Model : public FeatureModel {
        MovieCollection const *const movieCollection;

        Model(MovieCollection const *m)
            : movieCollection(m) {}

        virtual QString title() const override { return movieCollection->name(); }

        virtual QString titleLabel() const override { return QObject::tr("Name"); }

        virtual QImage image() const override { return movieCollection->image(); }

        virtual optional<Uuid> linkedUuid() const override { return movieCollection->uuid(); }

        virtual QString description() const override { return movieCollection->description(); }

        virtual QString descriptionLabel() const override { return QObject::tr("Overview"); }

        virtual QString originalTitle() const override { return movieCollection->originalName(); }

        virtual QDate releaseDate() const override { return movieCollection->date(); }

        virtual BelongGroups belongsTo() const override {
            BelongGroups res;
            auto co = CollectionsTable::instance()->collectionIdsOfCollectable(
                {movieCollection->uuid(), Collectable::Type::movie});
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

    MovieCollection const *const movieCollection;
    unique_ptr<MovieCollectionWidget> widget;
    unique_ptr<Model> model;

    Detail(Presentation const &p)
        : movieCollection(safeCast<MovieCollection>(MovieObjectItem::object(p))) {}
};

MovieCollectionItem::MovieCollectionItem(Presentation const &p)
    : MovieObjectItem(p)
    , d(GREU_MKDET(p)) {
    d->model = make_unique<Detail::Model>(d->movieCollection);
}

MovieCollection const *MovieCollectionItem::movieCollection() const {
    return d->movieCollection;
}

PresentationWidget *MovieCollectionItem::widget() const {
    if(!d->widget)
        d->widget = make_unique<MovieCollectionWidget>(this);
    return d->widget.get();
}

FeatureModel *MovieCollectionItem::featureModel() const {
    return d->model.get();
}

unique_ptr<FeatureModel> MovieCollectionItem::featureModel(MovieCollection *m) {
    return make_unique<Detail::Model>(m);
}

} // namespace greu::gaion
