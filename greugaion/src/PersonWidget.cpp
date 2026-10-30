#include "PersonWidget.h"
#include <QLabel>
#include <QScrollArea>
#include <QTabWidget>
#include <QVBoxLayout>
#include "Dispatcher.h"
#include "LinkController.h"
#include "PersonItem.h"
#include "PlayableFileWidgets.h"
#include "Styles.h"
#include "greucore/include/GuardedPtr.h"
#include "greumovies/include/Episode.h"
#include "greumovies/include/Movie.h"
#include "greumovies/include/Person.h"
#include "greumovies/include/Series.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;
using namespace movies;

struct PersonWidget::Detail {
    GuardedPtr<QLabel> aliases;
    GuardedPtr<QTabWidget> creditsTabWidget;
    struct CreditEntry {
        GuardedPtr<QWidget> widget;
        GuardedPtr<QHBoxLayout> mainLayout;
        GuardedPtr<QVBoxLayout> rightLayout;
        GuardedPtr<QLabel> year, movieObject, jobs;
        CreditEntry(QString const &y, QString const &mo, QString const &j) {
            widget = new QWidget;
            mainLayout = new QHBoxLayout;
            widget->setLayout(mainLayout.get());
            rightLayout = new QVBoxLayout;
            addNewWidget(year, y, mainLayout.get());
            mainLayout->addLayout(rightLayout.get());
            addNewWidget(movieObject, mo, rightLayout.get());
            ui::connect(movieObject.get(), [](auto s) { GREU_D->linkClicked(s); });
            addNewWidget(jobs, j, rightLayout.get());
            ui::connect(jobs.get(), [](auto s) { GREU_D->linkClicked(s); });
            mainLayout->addStretch(1);
        }
    };
    struct CreditsTabEntry {
        QString const title;
        GuardedPtr<QWidget> widget;
        GuardedPtr<QVBoxLayout> layout;
        vector<CreditEntry> entries;
        CreditsTabEntry(QString const &t)
            : title(t) {
            widget = new QWidget;
            layout = new QVBoxLayout;
            widget->setLayout(layout.get());
        }
    };
    PersonItem const *personItem;
    vector<CreditsTabEntry> creditsTabs;
    Person const *person;

    Detail(PersonItem const *p)
        : personItem(p) {
        person = personItem->person();
    }

    void addAtHeadInfoEnd(QVBoxLayout *layout) {
        addNewWidget(aliases,
                     QString("%1<br>%2")
                         .arg(GREU_STYL->subHeader(QObject::tr("Also known as")))
                         .arg(GREU_STYL->overview(ranges::to<QString>(
                             views::join_with(person->aliases(), string(", "))))),
                     layout);
        aliases->setWordWrap(true);
    }

    void addAfterHead(QVBoxLayout *layout, PersonWidget *that) {
        auto credits = person->categorizedCredits();
        if (credits.size() > 0) {
            addNewWidget(creditsTabWidget, layout);
            for (auto const &cc : credits) {
                CreditsTabEntry tabEntry{cc.category};
                creditsTabWidget->addTab(tabEntry.widget.get(), tabEntry.title);
                for (auto c : cc.credits) {
                    if (c.movie) {
                        auto ce = CreditEntry{GREU_STYL->title(c.year),
                                              GREU_STYL->titleLink(that->linkController()->buildLink(
                                                                       Presentation{
                                                                           c.movie->uuid()}),
                                                                   c.movie->name()),
                                              GREU_STYL->job(c.job)};
                        tabEntry.layout->addWidget(ce.widget.get());
                        tabEntry.entries.push_back(std::move(ce));
                    } else {
                        Uuids eu;
                        ranges::transform(c.episodes, back_inserter(eu), [](Episode *e) {
                            return e->uuid();
                        });
                        Presentation pres{"",
                                          ListType::Features,
                                          {{QObject::tr("%1 of %2 in %3")
                                                .arg(c.episodes.size() == 1
                                                         ? QObject::tr("Episode")
                                                         : QObject::tr("Episodes"))
                                                .arg(person->name())
                                                .arg(c.series->name()),
                                            eu}}};
                        auto ce = CreditEntry{
                            GREU_STYL->title(c.year),
                            GREU_STYL->titleLink(that->linkController()->buildLink(
                                                     Presentation{c.series->uuid()}),
                                                 c.series->name()),
                            GREU_STYL->job(c.job)
                                + GREU_STYL->jobLink(that->linkController()->buildLink(pres),
                                                     QString(" (%1 %2)")
                                                         .arg(QString::number(c.episodes.size()))
                                                         .arg(c.episodes.size() == 1
                                                                  ? QObject::tr("Episode")
                                                                  : QObject::tr("Episodes")))};
                        tabEntry.layout->addWidget(ce.widget.get());
                        tabEntry.entries.push_back(std::move(ce));
                    }
                }
                tabEntry.layout->addStretch(1);
                creditsTabs.push_back(std::move(tabEntry));
            }
        }
    }
};

PersonWidget::PersonWidget(PersonItem const *p)
    : MovieObjectWidget(p)
    , d(GREU_MKDET(p)) {
    init();
}

void PersonWidget::addAtHeadInfoEnd(QVBoxLayout *l) {
    d->addAtHeadInfoEnd(l);
}

void PersonWidget::addAfterHead(QVBoxLayout *l) {
    d->addAfterHead(l, this);
}

} // namespace greu::gaion
