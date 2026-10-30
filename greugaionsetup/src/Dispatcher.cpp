#include "Dispatcher.h"
#include <QMessageBox>
#include <QTreeView>
#include "Actions.h"
#include "CollectionDialog.h"
#include "IdPopup.h"
#include "ImageImporter.h"
#include "MainWindow.h"
#include "OwnMovieEditor.h"
#include "greucore/include/GuardedPtr.h"
#include "greugaion/include/NavigationController.h"
#include "greugaion/include/PresentationItem.h"
#include "greumovies/include/Cleanser.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/CollectionsTable.h"
#include "greumovies/include/CreditAssignmentsTable.h"
#include "greumovies/include/CreditsTable.h"
#include "greumovies/include/Episode.h"
#include "greumovies/include/EpisodesTable.h"
#include "greumovies/include/File.h"
#include "greumovies/include/FilesTable.h"
#include "greumovies/include/GenresDefinitionTable.h"
#include "greumovies/include/GenresTable.h"
#include "greumovies/include/JobsTable.h"
#include "greumovies/include/Movie.h"
#include "greumovies/include/MovieCollection.h"
#include "greumovies/include/MovieCollectionsTable.h"
#include "greumovies/include/MoviesTable.h"
#include "greumovies/include/Person.h"
#include "greumovies/include/PersonsTable.h"
#include "greumovies/include/Season.h"
#include "greumovies/include/SeasonsTable.h"
#include "greumovies/include/Series.h"
#include "greumovies/include/SeriesTable.h"
#include "greumovies/include/Settings.h"
#include "greumpv/include/Player.h"
#include "greumpv/include/Video.h"
#include "greunet/include/Exception.h"
#include "greuui/include/Action.h"
#include "greuui/include/BusyCursor.h"
#include "greuui/include/FileSystemModel.h"

using namespace std;

namespace greu::gaion::setup {

using namespace core;
using namespace tmdb;
using namespace movies;
using namespace ui;
using namespace mpv;

struct Dispatcher::Detail {
    MainWindow mainWindow;
    unique_ptr<Actions> actions;
    unique_ptr<Player> player;

    void initialize(Dispatcher *that) {
        mainWindow.setupUi();
        actions = make_unique<Actions>();
        actions->playFile().setEnabled(false);
        actions->search().setEnabled(false);
        actions->basePathChanged().connect([](auto newPath) {
            GREU_M->fileModel()->setRootPath(newPath);
            GREU_M->fileTree()->setRootIndex(GREU_M->fileModel()->index(newPath));
        });
        mainWindow.fileSelected().connect([](QFileInfo const &file) {
            if (file.isFile() && File::isUsed(file.absoluteFilePath())) {
                auto f = FilesTable::instance()->readForFileSystem(file.absoluteFilePath());
                if (f) {
                    GREU_NAVC->setCurrent(Presentation{f->video()->uuid()});
                    GREU_M->setMediumWidget(GREU_NAVC->currentItem()->widget());
                    return;
                }
            }
            GREU_NAVC->setCurrent({});
            GREU_M->setMediumWidget(nullptr);
        });
        mainWindow.assignMovieRequested().connect([this](auto fi) { assignFile(fi, true); });
        mainWindow.assignEpisdoeRequested().connect([this](auto fi) { assignFile(fi, false); });
        mainWindow.assignOwnMovieRequested().connect([this](auto fi) { assignOwn(fi); });
        mainWindow.unassignFileRequested().connect(
            [this, that](auto fi) { unassignFile(fi, that); });
        mainWindow.updateFileRequested().connect([this, that](auto fi) { updateFile(fi, that); });
        mainWindow.playFileRequested().connect([this](auto fi) {
            player = make_unique<Player>(fi);
            ScopedConnection conn = player->finished().connect([this] { player.reset(nullptr); });
            player->play();
        });
        mainWindow.show();
    }

    void updateFile(auto fi, Dispatcher *that) {
        auto file = FilesTable::instance()->readForFileSystem(fi.absoluteFilePath());
        if (!file->isUsed())
            return;
        auto obj = file->video();
        if (!obj)
            return;
        IdPopup::Id id;
        auto imgx = obj->imageX();
        bool isMovie;
        if (obj->objectType() == MovieObject::ObjectType::movie) {
            auto movie = safeCast<Movie>(obj);
            id.id = movie->tmdbId();
            id.variantTitle = movie->name();
            isMovie = true;
        } else if (obj->objectType() == MovieObject::ObjectType::episode) {
            auto episode = safeCast<Episode>(obj);
            auto key = episode->tmdbId();
            id.id = key.seriesId;
            id.seasonNumber = key.seasonNumber;
            id.variantSeasonNumber = episode->variantSeasonNumber();
            id.episodeNumber = key.episodeNumber;
            id.variantEpisodeNumber = episode->variantEpisodeNumber();
            id.variantTitle = episode->name();
            isMovie = false;
        } else
            return;
        auto uuid = obj->uuid();
        unassignFile(fi, that, false);
        assignFile(fi, isMovie, id, false);
        file = FilesTable::instance()->readForFileSystem(fi.absoluteFilePath());
        if (!file->isUsed())
            return;
        obj = file->video();
        if (!obj)
            return;
        if (!obj)
            return;
        if (obj->imageX().isEmpty())
            obj->setImageX(imgx);
        obj->save();
        GREU_NAVC->setCurrent(Presentation{obj->uuid()});
        GREU_D->removeUnused();
    }

    void assignFile(QFileInfo const &fi, bool isMovie) {
        IdPopup pop{fi, isMovie};
        if (pop.exec())
            assignFile(fi, isMovie, pop.id());
    }

    void assignFile(QFileInfo const &fi, bool isMovie, IdPopup::Id const &id, bool navigate = true) {
        BusyCursor _;
        unique_ptr<Presentation> pres;
        try {
            auto file = FilesTable::instance()->readForFileSystem(fi.absoluteFilePath());
            mpv::Video v{fi};
            file->setRuntimeSecs(v.durationSecs());
            if (isMovie) {
                auto m = Movie::createFromTmdb(id.id, id.variantTitle);
                file->setVideo(m);
                m->save();
                pres = make_unique<Presentation>(m->uuid());
            } else {
                auto e = Episode::createFromTmdb({id.id,
                                                  (int32_t) id.seasonNumber,
                                                  (int32_t) id.episodeNumber},
                                                 id.variantSeasonNumber,
                                                 id.variantEpisodeNumber,
                                                 id.variantTitle);
                file->setVideo(e);
                e->save();
                pres = make_unique<Presentation>(e->uuid());
            }
            file->save();
            if (navigate) {
                GREU_NAVC->setCurrent(*pres);
                GREU_D->removeUnused();
            }
        } catch (net::Exception &e) {
            if (e.code() == QNetworkReply::NetworkError::ContentNotFoundError)
                QMessageBox::warning(nullptr,
                                     QObject::tr("Error"),
                                     QObject::tr("Object not found in TMDB!"));
        }
    }

    void unassignFile(QFileInfo const &fi, Dispatcher *that, bool navigate = true) {
        auto file = FilesTable::instance()->readForFileSystem(fi.absoluteFilePath());
        if (!file->isUsed())
            return;
        file->setDeleted();
        file->save();
        that->removeUnused();
        if (navigate) {
            GREU_NAVC->setCurrent(Presentation());
            GREU_D->removeUnused();
        }
    }

    void assignOwn(QFileInfo const &fi) {
        GuardedPtr<OwnMovieEditor> ed = new OwnMovieEditor;
        if (ed->exec()) {
            BusyCursor _;
            auto om = ed->ownMovie();
            auto file = FilesTable::instance()->readForFileSystem(fi.absoluteFilePath());
            mpv::Video v{fi};
            file->setRuntimeSecs(v.durationSecs());
            auto m = Movie::createFromOwnMovie(om);
            file->setVideo(m);
            m->save();
            file->save();
            GREU_NAVC->setCurrent(Presentation(m->uuid()));
            GREU_D->removeUnused();
        }
    }
};

Dispatcher::Dispatcher()
    : d(GREU_MKDET()) {}

ui::MainWindow *Dispatcher::mainWindow() const {
    return &d->mainWindow;
}

void Dispatcher::initialize() {
    movies::Settings::instance();
    JobsTable::instance();
    CreditsTable::instance();
    FilesTable::instance();
    GenresTable::instance();
    PersonsTable::instance();
    EpisodesTable::instance();
    SeasonsTable::instance();
    SeriesTable::instance();
    MoviesTable::instance();
    MovieCollectionsTable::instance();
    CollectionsTable::instance();
    removeUnused();
    JobsTable::instance()->refresh();
    GenresDefinitionTable::instance()->refresh();
    d->initialize(this);
    gaion::Dispatcher::initialize();
}

void Dispatcher::removeUnused() {
    auto c = Cleanser::instance();
    c->add(EpisodesTable::instance());
    c->add(SeasonsTable::instance());
    c->add(SeriesTable::instance());
    c->add(MoviesTable::instance());
    c->add(MovieCollectionsTable::instance());
    c->add(CreditsTable::instance());
    c->add(CreditAssignmentTable::instance());
    c->add(PersonsTable::instance());
    c->add(FilesTable::instance());
    c->clean();
}

void Dispatcher::deleteCollection() {
    core::GuardedPtr<CollectionDialog> input = new CollectionDialog;
    if (input->exec()) {
        auto coll = input->collection();
        coll->setDeleted();
        coll->save();
    }
}

void Dispatcher::addMediumToCollection() {
    auto const pres = GREU_NAVC->current();
    if (!pres->isSingle())
        return;
    auto uuid = pres->singleUuid();
    Collectable *c = dynamic_cast<Collectable *>(Object::existingObject(uuid));
    if (!c)
        return;
    GuardedPtr<CollectionDialog> input = new CollectionDialog;
    if (input->exec()) {
        auto coll = input->collection();
        coll->addCollectableKey(c->key());
        coll->setNew();
        coll->save();
    }
}

void Dispatcher::removeMediumFromCollection() {
    auto const pres = GREU_NAVC->current();
    if (!pres->isSingle())
        return;
    auto uuid = pres->singleUuid();
    Collectable *c = dynamic_cast<Collectable *>(Object::existingObject(uuid));
    if (!c)
        return;
    GuardedPtr<CollectionDialog> input = new CollectionDialog;
    if (input->exec()) {
        auto coll = input->collection();
        coll->removeCollectableKey(c->key());
        coll->setNew();
        coll->save();
    }
}

void Dispatcher::setImageForObject() {
    auto const pres = GREU_NAVC->current();
    if (!pres->isSingle())
        return;
    auto uuid = pres->singleUuid();
    auto m = dynamic_cast<MovieObject *>(Object::existingObject(uuid));
    if (!m)
        return;
    auto img = ImageImporter::import();
    m->setImageX(img);
    m->setNew();
    m->save();
}

void Dispatcher::updateObject() {
    auto const pres = GREU_NAVC->current();
    if (!pres->isSingle())
        return;
    auto uuid = pres->singleUuid();
    auto m = dynamic_cast<MovieObject *>(Object::existingObject(uuid));
    if (!m)
        return;
    switch (m->objectType()) {
    case MovieObject::ObjectType::episode: {
        auto e = safeCast<Episode>(m);
        if (auto fo = e->file())
            d->updateFile(fo.value()->fileInfo(), this);
    } break;
    case MovieObject::ObjectType::movie: {
        auto mv = safeCast<Movie>(m);
        if (auto fo = mv->file())
            d->updateFile(fo.value()->fileInfo(), this);
    } break;
    default:
        m->update();
    }
}

void Dispatcher::shutDown() {
    removeUnused();
}

} // namespace greu::gaion::setup
