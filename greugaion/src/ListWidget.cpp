#include "ListWidget.h"
#include <QGridLayout>
#include <QScrollArea>
#include <QScrollBar>
#include <QTabWidget>
#include "Dispatcher.h"
#include "FeatureModel.h"
#include "FeatureWidget.h"
#include "LinkController.h"
#include "ListItem.h"
#include "MovieObjectItem.h"
#include "PlayableFileWidgets.h"
#include "Styles.h"
#include "greucore/include/GuardedPtr.h"
#include "greumovies/include/CreditHolder.h"
#include "greumovies/include/EmptyImages.h"
#include "greumovies/include/File.h"
#include "greumovies/include/Person.h"
#include "greumovies/include/Settings.h"
#include "greumovies/include/creditUtils.h"
#include "greuui/include/ImageButton.h"
#include "greuui/include/connect.h"
#include "greuui/include/imageTransformation.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::gaion {

using namespace movies;
using namespace core;
using namespace ui;

struct ListWidget::Detail {
    struct TilesWidget : public QScrollArea {
        struct Entry {
            GuardedPtr<QWidget> widget;
            GuardedPtr<QVBoxLayout> layout;
            GuardedPtr<ImageButton> picture;
            GuardedPtr<QLabel> title, percent;
            ScopedConnection connFinished;

            Entry(FeatureModel const *f, TilesWidget *that, ListWidget *listWidget) {
                auto linkedUuid = f->linkedUuid();
                assert(linkedUuid);
                auto link = listWidget->linkController()->buildLink(
                    Presentation{{linkedUuid.value()}});
                auto w = GREU_STYL->width() / (that->tilesPerLine + 0.5);
                auto h = w * 1.5;
                widget = new QWidget;
                layout = new QVBoxLayout;
                layout->setAlignment(Qt::AlignVCenter);
                widget->setLayout(layout.get());
                widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
                picture = new ImageButton;
                layout->addWidget(picture.get());
                picture->setImage(scale(f->image(), w, h, *that, ImageOrientation::portrait), link);
                picture->clicked().connect([](auto s) { GREU_D->linkClicked(s); });
                addNewWidget(title, GREU_STYL->titleTileLink(link, f->title()), layout.get());
                title->setWordWrap(true);
                title->setAlignment(Qt::AlignHCenter);
                ui::connect(title.get(), [](auto s) { GREU_D->linkClicked(s); });
                auto file = f->file();
                if (file && file->lastPosition() > 0) {
                    percent = new QLabel(percentText(f->file()));
                    percent->setAlignment(Qt::AlignHCenter);
                } else {
                    percent = new QLabel;
                }
                layout->addWidget(percent.get());
                connFinished
                    = GREU_DC(gaion::Dispatcher)->playingFinished().connect([this, file](File *f) {
                          if (f != file)
                              return;
                          percent->setText(percentText(f));
                      });
            }

            QString percentText(File const *file) {
                return GREU_STYL->subTitleTile(QString("%1%").arg(file->percentViewed()));
            }
        };

        GuardedPtr<QWidget> widget;
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<QVBoxLayout> vertLayout;
        GuardedPtr<QHBoxLayout> mainLayout;
        uint tilesPerLine;
        vector<Entry> entries;

        TilesWidget(Presentation::GroupTab const &tab, ListWidget *listWidget) {
            setWidgetResizable(true);
            widget = new QWidget(this);
            mainLayout = new QHBoxLayout;
            widget->setLayout(mainLayout.get());
            vertLayout = new QVBoxLayout;
            layout = new QGridLayout;
            vertLayout->addLayout(layout.get());
            mainLayout->addLayout(vertLayout.get());
            widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
            tilesPerLine = movies::Settings::instance()->tilesPerRowBig();
            layout->setHorizontalSpacing(GREU_STYL->width() / (tilesPerLine * 40));
            layout->setVerticalSpacing(tilesPerLine * 1.5);
            size_t row = 0;
            size_t column = 0;
            for (auto const &u : tab.uuids) {
                Entry entry(MovieObjectItem::create(u)->featureModel(), this, listWidget);
                layout->addWidget(entry.widget.get(), row, column);
                if (column == tilesPerLine - 1) {
                    column = 0;
                    ++row;
                } else {
                    ++column;
                }
                entries.push_back(std::move(entry));
            }
            PresentationWidget::gridEven(layout.get());
            vertLayout->addStretch(2);
            mainLayout->addStretch(2);
            setWidget(widget.get());
        }
    };

    struct FeatureWidget : public QScrollArea {
        GuardedPtr<QWidget> widget;
        GuardedPtr<QVBoxLayout> layout;
        vector<gaion::FeatureWidget> entries;

        FeatureWidget(Presentation::GroupTab const &tab, ListWidget *listWidget) {
            setWidgetResizable(true);
            widget = new QWidget(this);
            layout = new QVBoxLayout;
            widget->setLayout(layout.get());
            for (auto const &u : tab.uuids) {
                auto entry = gaion::FeatureWidget(listWidget,
                                                  MovieObjectItem::create(u)->featureModel(),
                                                  true,
                                                  {},
                                                  false);
                layout->addWidget(entry.widget());
                entries.push_back(std::move(entry));
            }
            layout->addStretch(2);
            setWidget(widget.get());
        }
    };

    struct CreditsWidget : public QScrollArea {
        GuardedPtr<QWidget> widget, cast, crew;
        GuardedPtr<QHBoxLayout> mainLayout;
        GuardedPtr<QVBoxLayout> castLayout, crewLayout;

        struct EntryTitle {
            GuardedPtr<QLabel> title;
        };
        struct EntryPerson {
            GuardedPtr<QWidget> widget;
            GuardedPtr<QHBoxLayout> layoutH;
            GuardedPtr<QVBoxLayout> layoutV;
            GuardedPtr<QLabel> name, job;
            GuardedPtr<ImageButton> image;
            ScopedConnection conn{};
        };
        using Entry = variant<EntryTitle, EntryPerson>;
        vector<Entry> entries;

        CreditsWidget(Detail *that, ListWidget *listWidget) {
            setWidgetResizable(true);
            auto holder = safeCast<CreditHolder>(
                Object::existingObject(that->item->presentation().groupTabs.at(0).uuids.at(0)));
            widget = new QWidget(this);
            mainLayout = new QHBoxLayout;
            widget->setLayout(mainLayout.get());

            cast = new QWidget;
            castLayout = new QVBoxLayout;
            cast->setLayout(castLayout.get());
            mainLayout->addWidget(cast.get());
            crew = new QWidget;
            crewLayout = new QVBoxLayout;
            crew->setLayout(crewLayout.get());
            mainLayout->addWidget(crew.get());

            addCast(holder, listWidget);
            castLayout->addStretch(2);
            addCrew(holder, listWidget);
            crewLayout->addStretch(2);

            mainLayout->addStretch(2);

            setWidget(widget.get());
        }

        void addCast(CreditHolder const *holder, ListWidget *listWidget) {
            auto const cast = completeCast(holder);
            set<size_t> exclRows;
            size_t row = 0;
            EntryTitle e{
                new QLabel(GREU_STYL->title(QObject::tr("Cast (%1)").arg(sizeOfCredits(cast))))};
            e.title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            castLayout->addWidget(e.title.get());
            entries.push_back(std::move(e));
            exclRows.insert(row++);
            for (auto const &t : cast)
                addCredits(t.credits, castLayout.get(), listWidget, row);
        }

        void addCrew(CreditHolder const *holder, ListWidget *listWidget) {
            auto const crew = completeCrew(holder);
            set<size_t> exclRows;
            size_t row = 0;
            EntryTitle e{
                new QLabel(GREU_STYL->title(QObject::tr("Crew (%1)").arg(sizeOfCredits(crew))))};
            e.title->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
            crewLayout->addWidget(e.title.get());
            entries.push_back(std::move(e));
            exclRows.insert(row++);
            for (auto const &t : crew) {
                EntryTitle e{new QLabel(GREU_STYL->subTitleTile(t.department))};
                crewLayout->addWidget(e.title.get());
                exclRows.insert(row++);
                entries.push_back(std::move(e));
                addCredits(t.credits, crewLayout.get(), listWidget, row);
            }
        }

        void addCredits(vector<MediumCredit> const &credits,
                        QVBoxLayout *layout,
                        ListWidget *listWidget,
                        size_t &row) {
            for (auto const &c : credits) {
                EntryPerson e{new QWidget,
                              new QHBoxLayout,
                              new QVBoxLayout,
                              new QLabel(GREU_STYL->jobLink(listWidget->linkController()->buildLink(
                                                                Presentation(c.person->uuid())),
                                                            c.person->name())),
                              new QLabel(GREU_STYL->job(c.job)),
                              new ImageButton};
                e.widget->setLayout(e.layoutH.get());
                e.widget->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
                e.image->setImage(c.person->image().scaledToWidth(GREU_STYL->width() / 16,
                                                                  Qt::SmoothTransformation),
                                  listWidget->linkController()->buildLink(
                                      Presentation(c.person->uuid())));
                ui::connect(e.name.get(), [](QString const &s) { GREU_D->linkClicked(s); });
                e.conn = e.image->clicked().connect(
                    [](QString const &s) { GREU_D->linkClicked(s); });
                e.layoutH->addWidget(e.image.get(), 0, Qt::AlignTop);
                e.layoutH->addLayout(e.layoutV.get());
                e.layoutV->addWidget(e.name.get());
                e.layoutV->addWidget(e.job.get());
                e.layoutV->addStretch(1);
                e.layoutH->addStretch(1);
                layout->addWidget(e.widget.get());
                entries.push_back(std::move(e));
                ++row;
            }
        }
    };

    struct EmptyWidget : public QWidget {
        GuardedPtr<QVBoxLayout> layout;
        GuardedPtr<QLabel> label;
        EmptyWidget() {
            layout = new QVBoxLayout;
            setLayout(layout.get());
            label = new QLabel(QObject::tr("No entries"));
            layout->addWidget(label.get());
        }
    };

    ListItem const *item;
    GuardedPtr<QWidget> widget;
    GuardedPtr<QVBoxLayout> layout;
    vector<GuardedPtr<QScrollArea>> entries;
    QScrollArea *singleScroll = nullptr;

    Detail(ListItem const *i)
        : item(i) {}

    void init(ListWidget *listWidget) {
        if (!item->presentation().hasUuid()) {
            widget = new EmptyWidget;
            return;
        }
        auto p = item->presentation();
        switch (p.listType) {
        case ListType::Tiles:
        case ListType::Features: {
            if (p.groupTabs.size() == 1 && p.groupTabs.at(0).title.isEmpty()) {
                widget = new QWidget;
                layout = new QVBoxLayout;
                widget->setLayout(layout.get());
            } else {
                widget = new QTabWidget;
            }
            for (auto const &g : p.groupTabs) {
                GuardedPtr<QScrollArea> w;
                if (p.listType == ListType::Features)
                    w = new FeatureWidget(g, listWidget);
                else
                    w = new TilesWidget(g, listWidget);
                if (layout) {
                    layout->addWidget(w.get());
                    singleScroll = w.get();
                } else {
                    dynamic_cast<QTabWidget *>(widget.get())->addTab(w.get(), g.title);
                }
                entries.push_back(std::move(w));
            }
        } break;
        case ListType::AllCredits:
            widget = new CreditsWidget(this, listWidget);
            break;
        case ListType::None:
            assert(false);
        }
    };

    using ScrollPos = pair<int, int>;
    struct TabPos {
        int tab;
        ScrollPos pos;
    };

    any position() const {
        auto p = item->presentation();
        switch (p.listType) {
        case ListType::Tiles:
        case ListType::Features: {
            if (singleScroll) {
                ScrollPos res;
                res.first = singleScroll->verticalScrollBar()->value();
                res.second = singleScroll->horizontalScrollBar()->value();
                return res;
            } else {
                auto tabs = safeCast<QTabWidget>(widget.get());
                TabPos res;
                res.tab = tabs->currentIndex();
                auto scroll = safeCast<QScrollArea>(tabs->currentWidget());
                res.pos.first = scroll->verticalScrollBar()->value();
                res.pos.second = scroll->horizontalScrollBar()->value();
                return res;
            }
        } break;
        case ListType::AllCredits: {
            auto scroll = safeCast<QScrollArea>(widget.get());
            ScrollPos res;
            res.first = scroll->verticalScrollBar()->value();
            res.second = scroll->horizontalScrollBar()->value();
            return res;
        } break;
        case ListType::None:
            assert(false);
        }
        return false;
    }

    void setPosition(any const &v) {
        auto p = item->presentation();
        switch (p.listType) {
        case ListType::Tiles:
        case ListType::Features: {
            if (singleScroll) {
                auto pos = any_cast<ScrollPos>(v);
                singleScroll->verticalScrollBar()->setValue(pos.first);
                singleScroll->horizontalScrollBar()->setValue(pos.second);
            } else {
                auto tabs = safeCast<QTabWidget>(widget.get());
                auto pos = any_cast<TabPos>(v);
                tabs->setCurrentIndex(pos.tab);
                auto scroll = safeCast<QScrollArea>(tabs->currentWidget());
                scroll->verticalScrollBar()->setValue(pos.pos.first);
                scroll->horizontalScrollBar()->setValue(pos.pos.second);
            }
        } break;
        case ListType::AllCredits: {
            auto scroll = safeCast<QScrollArea>(widget.get());
            auto pos = any_cast<ScrollPos>(v);
            scroll->verticalScrollBar()->setValue(pos.first);
            scroll->horizontalScrollBar()->setValue(pos.second);
        } break;
        case ListType::None:
            assert(false);
        }
    }
};

ListWidget::ListWidget(ListItem const *i)
    : d(GREU_MKDET(i)) {
    d->init(this);
}

QWidget *ListWidget::widget() const {
    return d->widget.get();
}

ListItem const *ListWidget::listItem() const {
    return d->item;
}

void ListWidget::setPosition(any const &v) {
    d->setPosition(v);
}

any ListWidget::position() const {
    return d->position();
}

} // namespace greu::gaion
