#include "SeasonItem.h"
#include <QImage>
#include "FeatureModel.h"
#include "SeasonWidget.h"
#include "greumovies/include/Season.h"
#include "greumovies/include/Series.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct SeasonItem::Detail {
    struct Model : public FeatureModel {
        Season const *const season;

        Model(Season const *s)
            : season(s) {}

        virtual QString title() const override { return season->name(); }

        virtual QString titleLabel() const override { return QObject::tr("Name"); }

        virtual QImage image() const override { return season->image(); }

        virtual optional<Uuid> linkedUuid() const override { return season->uuid(); }

        virtual QString description() const override { return season->description(); }

        virtual QString descriptionLabel() const override { return QObject::tr("Overview"); }

        virtual QString originalTitle() const override { return season->originalName(); }

        virtual QDate releaseDate() const override { return season->date(); }

        virtual BelongGroups belongsTo() const override {
            BelongGroups res;
            auto series = season->series();
            BelongGroup g;
            g.type = QObject::tr("series");
            g.entries.push_back({series->name(), series->uuid()});
            res.push_back(g);
            return res;
        }
    };

    Season const *const season;
    unique_ptr<SeasonWidget> widget;
    unique_ptr<Model> model;

    Detail(Presentation const &p)
        : season(safeCast<Season>(MovieObjectItem::object(p))) {}
};

SeasonItem::SeasonItem(Presentation const &p)
    : MovieObjectItem(p)
    , d(GREU_MKDET(p)) {
    d->model = make_unique<Detail::Model>(d->season);
}

Season const *SeasonItem::season() const {
    return d->season;
}

PresentationWidget *SeasonItem::widget() const {
    if (!d->widget)
        d->widget = make_unique<SeasonWidget>(this);
    return d->widget.get();
}

FeatureModel *SeasonItem::featureModel() const {
    return d->model.get();
}

unique_ptr<FeatureModel> SeasonItem::featureModel(movies::Season const *s) {
    return make_unique<Detail::Model>(s);
}

} // namespace greu::gaion
