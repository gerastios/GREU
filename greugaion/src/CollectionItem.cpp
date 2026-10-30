#include "CollectionItem.h"
#include <QImage>
#include "CollectionWidget.h"
#include "FeatureModel.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/CollectionsTable.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct CollectionItem::Detail {
    struct Model : public FeatureModel {
        Collection const *const collection;

        Model(Collection const *c)
            : collection(c) {}

        virtual QString title() const override { return collection->nameInCollection(); }

        virtual QString titleLabel() const override { return QObject::tr("Name"); }

        virtual QImage image() const override { return collection->image(); }

        virtual optional<Uuid> linkedUuid() const override { return collection->uuid(); }

        virtual QString description() const override { return collection->description(); }

        virtual QString descriptionLabel() const override { return QObject::tr("Description"); }

        virtual BelongGroups belongsTo() const override {
            BelongGroups res;
            auto co = CollectionsTable::instance()->collectionIdsOfCollectable(
                {collection->uuid(), Collectable::Type::movie});
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

    Collection const *const collection;
    unique_ptr<CollectionWidget> widget;
    unique_ptr<Model> model;

    Detail(Presentation const &p)
        : collection(safeCast<Collection>(MovieObjectItem::object(p))) {}
};

CollectionItem::CollectionItem(Presentation const &p)
    : MovieObjectItem(p)
    , d(GREU_MKDET(p)) {
    d->model = make_unique<Detail::Model>(d->collection);
}

Collection const *CollectionItem::collection() const {
    return d->collection;
}

PresentationWidget *CollectionItem::widget() const {
    if(!d->widget)
        d->widget = make_unique<CollectionWidget>(this);
    return d->widget.get();
}

FeatureModel *CollectionItem::featureModel() const {
    return d->model.get();
}

unique_ptr<FeatureModel> CollectionItem::featureModel(Collection *c) {
    return make_unique<Detail::Model>(c);
}

} // namespace greu::gaion
