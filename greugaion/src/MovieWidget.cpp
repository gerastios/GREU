#include "MovieWidget.h"
#include "CreditWidget.h"
#include "MovieItem.h"
#include "greumovies/include/Movie.h"
#include <QVBoxLayout>
#include <QPushButton>

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;
using namespace movies;

struct MovieWidget::Detail {
    unique_ptr<CreditWidget> importantCast, importantCrew;
    CastCrewButton castCrew;
    MovieItem const *movieItem;
    Movie const *movie;

    Detail(MovieItem const *m)
        : movieItem(m) {
        movie = movieItem->movie();
    }

    void addAtHeadInfoEnd(QVBoxLayout *layout, MovieWidget *that) {
        importantCrew = make_unique<CreditWidget>(QObject::tr("Important crew"), movies::importantCrew(movie, 8),
                                                  CreditWidget::ListType::Names,
                                                  4,
                                                  that);
        layout->addWidget(importantCrew->widget());
    }

    void addAfterHead(QVBoxLayout *layout, MovieWidget *that) {
        importantCast = make_unique<CreditWidget>(QObject::tr("Important cast"), movies::importantCast(movie, 16),
                                                  CreditWidget::ListType::Pictures,
                                                  8,
                                                  that);
        layout->addWidget(importantCast->widget());
        castCrew = that->castCrewButton(movie, layout);
    }
};

MovieWidget::MovieWidget(MovieItem const *m)
    : MovieObjectWidget(m)
    , d(GREU_MKDET(m)) {
    init();
}

void MovieWidget::addAtHeadInfoEnd(QVBoxLayout *l) {
    d->addAtHeadInfoEnd(l, this);
}

void MovieWidget::addAfterHead(QVBoxLayout *l) {
    d->addAfterHead(l, this);
}

} // namespace greu::gaion
