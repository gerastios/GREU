#include "SeriesItem.h"
#include <QImage>
#include "FeatureModel.h"
#include "SeriesWidget.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/CollectionsTable.h"
#include "greumovies/include/Series.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct SeriesItem::Detail {
    struct Model : public FeatureModel {
        Series const *const series;

        Model(Series const *s)
            : series(s) {}

        virtual QString title() const override { return series->name(); }

        virtual QString titleLabel() const override { return QObject::tr("Name"); }

        virtual QImage image() const override { return series->image(); }

        virtual optional<Uuid> linkedUuid() const override { return series->uuid(); }

        virtual QString description() const override { return series->description(); }

        virtual QString descriptionLabel() const override { return QObject::tr("Overview"); }

        virtual QString originalTitle() const override { return series->originalName(); }

        virtual QString tagline() const override { return series->tagline(); }

        virtual QDate releaseDate() const override { return series->date(); }

        virtual set<QString> genres() const override { return series->genres(); }

        virtual BelongGroups belongsTo() const override {
            BelongGroups res;
            auto co = CollectionsTable::instance()->collectionIdsOfCollectable(
                {series->uuid(), Collectable::Type::series});
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

    Series const *const series;
    unique_ptr<SeriesWidget> widget;
    unique_ptr<Model> model;

    Detail(Presentation const &p)
        : series(safeCast<Series>(MovieObjectItem::object(p))) {}
};

SeriesItem::SeriesItem(Presentation const &p)
    : MovieObjectItem(p)
    , d(GREU_MKDET(p)) {
    d->model = make_unique<Detail::Model>(d->series);
}

Series const *SeriesItem::series() const {
    return d->series;
}

PresentationWidget *SeriesItem::widget() const {
    if(!d->widget)
        d->widget = make_unique<SeriesWidget>(this);
    return d->widget.get();
}

FeatureModel *SeriesItem::featureModel() const {
    return d->model.get();
}

std::unique_ptr<FeatureModel> SeriesItem::featureModel(movies::Series *s) {
    return make_unique<Detail::Model>(s);
}

} // namespace greu::gaion
