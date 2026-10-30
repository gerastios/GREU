#include "SeriesWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include "CollectionEntryWidget.h"
#include "CreditWidget.h"
#include "FeatureModel.h" // IWYU pragma: keep
#include "SeasonItem.h"
#include "SeriesItem.h"
#include "greumovies/include/Series.h"
#include "greumovies/include/creditUtils.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;
using namespace movies;

struct SeriesWidget::Detail {
    unique_ptr<CreditWidget> importantCast, creators;
    CastCrewButton castCrew;
    SeriesItem const *seriesItem;
    Series const *series;

    CollectionEntryWidget::Entries entries;

    Detail(SeriesItem const *s)
        : seriesItem(s) {
        series = seriesItem->series();
    }

    void addAtHeadInfoEnd(QVBoxLayout *layout, SeriesWidget *that) {
        creators = make_unique<CreditWidget>(QObject::tr("Created by"), movies::creators(series, 8),
                                             CreditWidget::ListType::Names,
                                             4,
                                             that);
        layout->addWidget(creators->widget());
    }

    void addAfterHead(QVBoxLayout *layout, SeriesWidget *that) {
        importantCast = make_unique<CreditWidget>(QObject::tr("Important cast"), movies::importantCast(series, 16),
                                                  CreditWidget::ListType::Pictures,
                                                  8,
                                                  that);
        layout->addWidget(importantCast->widget());
        castCrew = that->castCrewButton(series, layout);
        auto seasons = series->seasons();
        if (seasons.size() > 0) {
            for (auto const &season : series->seasons()) {
                CollectionEntryWidget::Entry e;
                e.featureModel = SeasonItem::featureModel(season);
                e.widget = make_unique<CollectionEntryWidget>(e.featureModel.get(), that);
                layout->addWidget(e.widget->widget());
                entries.push_back(std::move(e));
            }
        }
    }
};

SeriesWidget::SeriesWidget(SeriesItem const *s)
    : MovieObjectWidget(s)
    , d(GREU_MKDET(s)) {
    init();
}

void SeriesWidget::addAtHeadInfoEnd(QVBoxLayout *l) {
    d->addAtHeadInfoEnd(l, this);
}

void SeriesWidget::addAfterHead(QVBoxLayout *l) {
    d->addAfterHead(l, this);
}

} // namespace greu::gaion
