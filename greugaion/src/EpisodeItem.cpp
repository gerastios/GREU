#include "EpisodeItem.h"
#include <QImage>
#include "EpisodeWidget.h"
#include "FeatureModel.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/CollectionsTable.h"
#include "greumovies/include/Episode.h"
#include "greumovies/include/File.h"
#include "greumovies/include/FilesTable.h"
#include "greumovies/include/Season.h"
#include "greumovies/include/Series.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct EpisodeItem::Detail {
    struct Model : public FeatureModel {
        Episode const *const episode;

        Model(Episode const *e)
            : episode(e) {}

        virtual QString title() const override { return episode->name(); }

        virtual QString titleLabel() const override { return QObject::tr("Name"); }

        virtual QImage image() const override { return episode->image(); }

        virtual optional<Uuid> linkedUuid() const override { return episode->uuid(); }

        virtual QString description() const override { return episode->description(); }

        virtual QString descriptionLabel() const override { return QObject::tr("Overview"); }

        virtual QString originalTitle() const override { return episode->originalName(); }

        virtual File *file() const override {
            auto fo = episode->file();
            if (fo)
                return fo.value();
            return nullptr;
        }

        virtual QDate releaseDate() const override { return episode->date(); }

        virtual BelongGroups belongsTo() const override {
            BelongGroups res;
            {
                auto season = episode->season();
                BelongGroup g;
                g.type = QObject::tr("season");
                g.entries.push_back({season->name(), season->uuid()});
                res.push_back(g);
            }
            {
                auto series = episode->series();
                BelongGroup g;
                g.type = QObject::tr("series");
                g.entries.push_back({series->name(), series->uuid()});
                res.push_back(g);
            }
            auto co = CollectionsTable::instance()->collectionIdsOfCollectable(
                {episode->uuid(), Collectable::Type::episode});
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

    Episode const *const episode;
    unique_ptr<EpisodeWidget> widget;
    unique_ptr<Model> model;

    Detail(Presentation const &p)
        : episode(safeCast<Episode>(MovieObjectItem::object(p))) {}
};

EpisodeItem::EpisodeItem(Presentation const &p)
    : MovieObjectItem(p)
    , d(GREU_MKDET(p)) {
    d->model = make_unique<Detail::Model>(d->episode);
}

Episode const *EpisodeItem::episode() const {
    return d->episode;
}

PresentationWidget *EpisodeItem::widget() const {
    if(!d->widget)
        d->widget = make_unique<EpisodeWidget>(this);
    return d->widget.get();
}

FeatureModel *EpisodeItem::featureModel() const {
    return d->model.get();
}

unique_ptr<FeatureModel> EpisodeItem::featureModel(Episode *e) {
    return make_unique<Detail::Model>(e);
}

} // namespace greu::gaion
