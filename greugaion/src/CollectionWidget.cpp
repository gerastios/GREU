#include "CollectionWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "CollectionEntryWidget.h"
#include "CollectionItem.h"
#include "EpisodeItem.h"
#include "FeatureModel.h" // IWYU pragma: keep
#include "MovieCollectionItem.h"
#include "MovieItem.h"
#include "SeriesItem.h"
#include "greumovies/include/Collectable.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/Episode.h"
#include "greumovies/include/File.h"
#include "greumovies/include/Movie.h"
#include "greumovies/include/MovieCollection.h"
#include "greumovies/include/Series.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;
using namespace movies;

struct CollectionWidget::Detail {
    CollectionItem const *collectionItem;
    Collection const *collection;

    CollectionEntryWidget::Entries entries;

    Detail(CollectionItem const *m)
        : collectionItem(m) {
        collection = collectionItem->collection();
    }

    void addAfterHead(QVBoxLayout *layout, CollectionWidget *that) {
        auto keys = collection->collectableKeys();
        if (!keys.empty()) {
            for (auto const &key : keys) {
                auto collObj = dynamic_cast<Collectable *>(Object::existingObject(key.uuid));
                assert(collObj);
                CollectionEntryWidget::Entry e;
                switch (key.type) {
                case Collectable::Type::collection: {
                    auto o = safeCast<Collection>(collObj);
                    e.featureModel = CollectionItem::featureModel(o);
                } break;
                case Collectable::Type::episode: {
                    auto o = safeCast<Episode>(collObj);
                    e.featureModel = EpisodeItem::featureModel(o);
                } break;
                case Collectable::Type::movie: {
                    auto o = safeCast<Movie>(collObj);
                    e.featureModel = MovieItem::featureModel(o);
                } break;
                case Collectable::Type::movieCollection: {
                    auto o = safeCast<MovieCollection>(collObj);
                    e.featureModel = MovieCollectionItem::featureModel(o);
                } break;
                case Collectable::Type::series: {
                    auto o = safeCast<Series>(collObj);
                    e.featureModel = SeriesItem::featureModel(o);
                } break;
                }
                e.widget = make_unique<CollectionEntryWidget>(e.featureModel.get(),
                                                              that,
                                                              Collectable::typeString(key.type));
                layout->addWidget(e.widget->widget());
                entries.push_back(std::move(e));
            }
        }
    }
};

CollectionWidget::CollectionWidget(CollectionItem const *m)
    : MovieObjectWidget(m)
    , d(GREU_MKDET(m)) {
    init();
}

void CollectionWidget::addAfterHead(QVBoxLayout *l) {
    d->addAfterHead(l, this);
}

} // namespace greu::gaion
