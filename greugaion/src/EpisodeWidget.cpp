#include "EpisodeWidget.h"
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include "CreditWidget.h"
#include "EpisodeItem.h"
#include "greumovies/include/Episode.h"
#include "greumovies/include/File.h"
#include "greumovies/include/FilesTable.h"
#include "greumovies/include/creditUtils.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;
using namespace movies;

struct EpisodeWidget::Detail {
    unique_ptr<CreditWidget> guestStars, writers, directors;
    CastCrewButton castCrew;
    EpisodeItem const *episodeItem;
    Episode const *episode;

    Detail(EpisodeItem const *e)
        : episodeItem(e) {
        episode = episodeItem->episode();
    }

    void addAtHeadInfoEnd(QVBoxLayout *layout, EpisodeWidget *that) {
        writers = make_unique<CreditWidget>(QObject::tr("Written by"), movies::crewForJob(episode, "Writer", 0),
                                            CreditWidget::ListType::Names,
                                            8,
                                            that);
        layout->addWidget(writers->widget());
        directors = make_unique<CreditWidget>(QObject::tr("Directed by"), movies::crewForJob(episode, "Director", 0),
                                              CreditWidget::ListType::Names,
                                              8,
                                              that);
        layout->addWidget(directors->widget());
    }

    void addAfterHead(QVBoxLayout *layout, EpisodeWidget *that) {
        guestStars = make_unique<CreditWidget>(QObject::tr("Guest stars"), movies::guestStars(episode, 0),
                                               CreditWidget::ListType::Pictures,
                                               10,
                                               that);
        layout->addWidget(guestStars->widget());
        castCrew = that->castCrewButton(episode, layout);
    }
};

EpisodeWidget::EpisodeWidget(EpisodeItem const *e)
    : MovieObjectWidget(e)
    , d(GREU_MKDET(e)) {
    init();
}

void EpisodeWidget::addAtHeadInfoEnd(QVBoxLayout *l) {
    d->addAtHeadInfoEnd(l, this);
}

void EpisodeWidget::addAfterHead(QVBoxLayout *l) {
    d->addAfterHead(l, this);
}

} // namespace greu::gaion
