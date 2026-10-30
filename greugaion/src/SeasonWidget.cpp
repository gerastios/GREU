#include "SeasonWidget.h"
#include <QVBoxLayout>
#include <QPushButton>
#include "CollectionEntryWidget.h"
#include "CreditWidget.h"
#include "EpisodeItem.h"
#include "FeatureModel.h" // IWYU pragma: keep
#include "SeasonItem.h"
#include "greumovies/include/Episode.h"
#include "greumovies/include/Season.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;
using namespace movies;

struct SeasonWidget::Detail {
    unique_ptr<CreditWidget> importantCrew, importantCast;
    CastCrewButton castCrew;
    SeasonItem const *seasonItem;
    Season const *season;

    CollectionEntryWidget::Entries entries;

    Detail(SeasonItem const *s)
        : seasonItem(s) {
        season = seasonItem->season();
    }

    void addAtHeadInfoEnd(QVBoxLayout *layout, SeasonWidget *that) {
        importantCrew = make_unique<CreditWidget>(QObject::tr("Important crew"), movies::creators(season, 8),
                                                  CreditWidget::ListType::Names,
                                                  4,
                                                  that);
        layout->addWidget(importantCrew->widget());
    }

    void addAfterHead(QVBoxLayout *layout, SeasonWidget *that) {
        importantCast = make_unique<CreditWidget>(QObject::tr("Important cast"), movies::importantCast(season, 16),
                                                  CreditWidget::ListType::Pictures,
                                                  8,
                                                  that);
        layout->addWidget(importantCast->widget());
        castCrew = that->castCrewButton(season, layout);
        auto episodes = season->episodes();
        if (!episodes.empty()) {
            for (auto const &episode : season->episodes()) {
                CollectionEntryWidget::Entry e;
                e.featureModel = EpisodeItem::featureModel(episode);
                e.widget = make_unique<CollectionEntryWidget>(e.featureModel.get(), that);
                layout->addWidget(e.widget->widget());
                entries.push_back(std::move(e));
            }
        }
    }
};

SeasonWidget::SeasonWidget(SeasonItem const *s)
    : MovieObjectWidget(s)
    , d(GREU_MKDET(s)) {
    init();
}

void SeasonWidget::addAtHeadInfoEnd(QVBoxLayout *l) {
    d->addAtHeadInfoEnd(l, this);
}

void SeasonWidget::addAfterHead(QVBoxLayout *l) {
    d->addAfterHead(l, this);
}

} // namespace greu::gaion
