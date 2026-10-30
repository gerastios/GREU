#include "Dispatcher.h"
#include <QBuffer>
#include <QDir>
#include <QMessageBox>
#include "MainWindow.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/signalUtils.h"
#include "greugaion/include/Presentation.h"
#include "greugaion/include/PresentationItem.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/CollectionsTable.h"
#include "greumovies/include/EpisodesTable.h"
#include "greumovies/include/Movie.h"
#include "greumovies/include/MovieCollection.h"
#include "greumovies/include/MovieCollectionsTable.h"
#include "greumovies/include/MoviesTable.h"
#include "greumovies/include/PersonsTable.h"
#include "greumovies/include/SearchOptions.h"
#include "greumovies/include/SeasonsTable.h"
#include "greumovies/include/Series.h"
#include "greumovies/include/SeriesTable.h"
#include "greumovies/include/Settings.h"
#include "greuui/include/SplashScreen.h"
#include <boost/signals2.hpp>

using namespace std;
using namespace greu::core;
using namespace greu::movies;
using namespace greu::ui;
using namespace greu::gaion;

namespace {

template<class C>
void sort(
    Uuids &uuids, SplashScreen *splash, int hundred, int &count, function<C *(Uuid const &)> f) {
    for (auto const &uuid : uuids) {
        f(uuid);
        if (splash && ++count % 30 == 0)
            splash->setPercent(100 * count / hundred);
    }
    app::Dispatcher::sortByName(uuids);
}

template<typename C, set<QString> const &(C::*Method)() const>
Presentation::GroupTabs group(Uuids const &uuids, function<void(Uuids &)> sort) {
    map<QString, Uuids> groups;
    for (auto const &u : uuids) {
        auto gcs = (safeCast<C>(Object::existingObject(u))->*Method)();
        for (auto const &gc : gcs) {
            try {
                groups.at(gc).push_back(u);
            } catch (...) {
                groups[gc] = {u};
            }
        }
    }
    for (auto &g : groups)
        sort(g.second);
    Presentation::GroupTabs res;
    for (auto const &g : groups)
        res.push_back(Presentation::GroupTab{g.first, g.second});
    return res;
}

} // namespace

namespace greu::gaion::app {

struct Dispatcher::Detail {
    MainWindow mainWindow;
    Uuids allMovies;
    Uuids allSeries;
    Uuids allMovieCollections;
    Uuids allTopCollections;
    Collection *otherMovies = nullptr;
    bool noSplash = false;
    unique_ptr<ImageContainer> otherImageX;

    Detail() {
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        QPixmap{":/images/images/movies.jpg"}.save(&buffer, "JPG");
        otherImageX.reset(
            new ImageContainer(byteArray, ImageContainer::Type::JointPhotographicExpertsGroup));
    }

    void initialize(Dispatcher const *that) {
        if (!QDir(movies::Settings::instance()->pathPrefix()).isReadable()) {
            QMessageBox::critical(nullptr,
                                  QObject::tr("Error"),
                                  QObject::tr("Path to videos is not readable! Maybe you forgot to "
                                              "mount the storage medium."),
                                  QMessageBox::Ok,
                                  QMessageBox::Ok);
            std::exit(0);
        }
        QPixmap pixmap(":/images/images/gaion.jpg");
        GuardedPtr<SplashScreen> splash;
        {
            if (!noSplash) {
                splash = new SplashScreen(pixmap);
                ScopedConnection ctconn;
                splash->setAutoFillBackground(true);
                splash->show();
            }
            allMovies = MoviesTable::instance()->allMovieIds();
            allSeries = SeriesTable::instance()->allSeriesIds();
            allMovieCollections = MovieCollectionsTable::instance()->allMovieCollectionIds(true);
            allTopCollections = CollectionsTable::instance()->allTopCollectionIds();
            readOtherMovies();
            if (otherMovies)
                allTopCollections.push_back(otherMovies->uuid());
            int hundered = allSeries.size() + allMovies.size() + allMovieCollections.size()
                           + allTopCollections.size();
            int count = 0;
            sort<Series>(allSeries, splash.get(), hundered, count, Series::series);
            sort<Movie>(allMovies, splash.get(), hundered, count, Movie::movie);
            sort<MovieCollection>(allMovieCollections,
                                  splash.get(),
                                  hundered,
                                  count,
                                  MovieCollection::movieCollection);
            function<Collection *(Uuid const &)> f = static_cast<Collection *(*) (Uuid const &)>(
                Collection::collection);
            sort<Collection>(allTopCollections, splash.get(), hundered, count, f);
            if (!noSplash) {
                splash->setPercent(100);
                splash->finish(&mainWindow);
            }
        }
        mainWindow.setupUi();
        that->showMovies();
        mainWindow.showFullScreen();
    }

    void search(SearchOptions const &options) {
        Presentation pres;
        search(pres,
               options,
               SearchOptions::targetMovie,
               MoviesTable::instance(),
               QObject::tr("Movies"));
        search(pres,
               options,
               SearchOptions::targetMovieCollection,
               MovieCollectionsTable::instance(),
               QObject::tr("Movie collections"));
        search(pres,
               options,
               SearchOptions::targetCollection,
               CollectionsTable::instance(),
               QObject::tr("Collections"));
        search(pres,
               options,
               SearchOptions::targetSeries,
               SeriesTable::instance(),
               QObject::tr("Series"));
        search(pres,
               options,
               SearchOptions::targetSeason,
               SeasonsTable::instance(),
               QObject::tr("Seasons"));
        search(pres,
               options,
               SearchOptions::targetEpisode,
               EpisodesTable::instance(),
               QObject::tr("Episodes"));
        search(pres,
               options,
               SearchOptions::targetPerson,
               PersonsTable::instance(),
               QObject::tr("Persons"));
        if (pres.isEmpty()) {
            QMessageBox::information(nullptr,
                                     QObject::tr("Search"),
                                     QObject::tr("Sorry, nothing matches"));
        } else {
            pres.title = QObject::tr("Search result");
            pres.listType = ListType::Tiles;
            GREU_NAVC->setCurrent(pres);
        }
    }

    static void search(Presentation &presentation,
                       SearchOptions const &options,
                       SearchOptions::Target target,
                       Searchable *searchable,
                       QString const &title) {
        if (options.target & target) {
            auto objects = searchable->search(options);
            if (objects.size() > 0) {
                auto uuids = ranges::to<Uuids>(
                    views::transform(objects, [](auto o) { return o->uuid(); }));
                sortByName(uuids);
                presentation.groupTabs.push_back(
                    {QObject::tr("%1 (%2)").arg(title).arg(uuids.size()), uuids});
            }
        }
    }

    void readOtherMovies() {
        auto imids = MoviesTable::instance()->idsOfIndependentMovies();
        if (imids.size() == 0)
            return;
        otherMovies = Object::object<Collection>(Uuid{});
        otherMovies->setNew(false);
        otherMovies->setName(QObject::tr("Other movies"));
        otherMovies->setImageX(*otherImageX.get());
        for (auto &i : imids)
            otherMovies->addCollectableKey({i, Collectable::movie});
    }
};

Dispatcher::Dispatcher()
    : d(GREU_MKDET()) {}

void Dispatcher::initialize() {
    gaion::Dispatcher::initialize();
    d->initialize(this);
}

ui::MainWindow *Dispatcher::mainWindow() const {
    return &d->mainWindow;
}

void Dispatcher::showMovies() const {
    GREU_NAVC->setCurrent(Presentation{QObject::tr("All movies"), ListType::Tiles, d->allMovies});
}

void Dispatcher::showMovieCollections() const {
    GREU_NAVC->setCurrent(
        Presentation{QObject::tr("All movie collections"), ListType::Tiles, d->allMovieCollections});
}

void Dispatcher::showCollections() const {
    GREU_NAVC->setCurrent(
        Presentation{QObject::tr("All top collections"), ListType::Tiles, d->allTopCollections});
}

void Dispatcher::showSeries() const {
    GREU_NAVC->setCurrent(Presentation{QObject::tr("All TV series"), ListType::Tiles, d->allSeries});
}

void Dispatcher::showMoviesByGenre() const {
    GREU_NAVC->setCurrent(Presentation{QObject::tr("Movies by genre"),
                                       ListType::Tiles,
                                       group<Movie, &Movie::genres>(d->allMovies, sortByName)});
}

void Dispatcher::showSeriesByGenre() const {
    GREU_NAVC->setCurrent(Presentation{QObject::tr("Series by genre"),
                                       ListType::Tiles,
                                       group<Series, &Series::genres>(d->allSeries, sortByName)});
}

void Dispatcher::sortByName(Uuids &uuids) {
    ranges::sort(uuids, [](Uuid const &f, Uuid const &s) {
        return safeCast<MovieObject>(Object::existingObject(f))->name()
               < safeCast<MovieObject>(Object::existingObject(s))->name();
    });
}

void Dispatcher::sortByDate(Uuids &uuids) {
    ranges::sort(uuids, [](Uuid const &f, Uuid const &s) {
        return safeCast<MovieObject>(Object::existingObject(f))->date()
               < safeCast<MovieObject>(Object::existingObject(s))->date();
    });
}

void Dispatcher::search(SearchOptions const &o) {
    d->search(o);
}

void Dispatcher::setNoSplash(bool v) {
    d->noSplash = v;
}

} // namespace greu::gaion::app
