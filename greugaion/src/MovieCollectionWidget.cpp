#include "MovieCollectionWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include "CollectionEntryWidget.h"
#include "CreditWidget.h"
#include "FeatureModel.h" // IWYU pragma: keep
#include "MovieCollectionItem.h"
#include "MovieItem.h"
#include "greumovies/include/Movie.h"
#include "greumovies/include/MovieCollection.h"
#include "greumovies/include/creditUtils.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;
using namespace movies;

struct MovieCollectionWidget::Detail {
    unique_ptr<CreditWidget> importantCast, importantCrew;
    MovieCollectionItem const *movieCollectionItem;
    MovieCollection const *movieCollection;

    CollectionEntryWidget::Entries entries;

    Detail(MovieCollectionItem const *m)
        : movieCollectionItem(m) {
        movieCollection = movieCollectionItem->movieCollection();
    }

    void addAfterHead(QVBoxLayout *layout, MovieCollectionWidget *that) {
        auto movieIds = movieCollection->movieIds();
        vector<CreditHolder const *> holders;
        for (auto const &uuid : movieIds)
            holders.push_back(Movie::movie(uuid));
        importantCast = make_unique<CreditWidget>(QObject::tr("Important cast"), movies::importantCast(holders, 16),
                                                  CreditWidget::ListType::Pictures,
                                                  8,
                                                  that);
        layout->addWidget(importantCast->widget());
        importantCrew = make_unique<CreditWidget>(QObject::tr("Important crew"), movies::importantCrew(holders, 16),
                                                  CreditWidget::ListType::Pictures,
                                                  8,
                                                  that);
        layout->addWidget(importantCrew->widget());
        auto movies = movieCollection->movies();
        if (!movies.empty()) {
            for (auto const &movie : movies) {
                CollectionEntryWidget::Entry e;
                e.featureModel = MovieItem::featureModel(movie);
                e.widget = make_unique<CollectionEntryWidget>(e.featureModel.get(), that);
                layout->addWidget(e.widget->widget());
                entries.push_back(std::move(e));
            }
        }
    }
};

MovieCollectionWidget::MovieCollectionWidget(MovieCollectionItem const *m)
    : MovieObjectWidget(m)
    , d(GREU_MKDET(m)) {
    init();
}

void MovieCollectionWidget::addAfterHead(QVBoxLayout *l) {
    d->addAfterHead(l, this);
}

} // namespace greu::gaion
