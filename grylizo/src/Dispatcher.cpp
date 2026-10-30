#include "Dispatcher.h"
#include <QElapsedTimer>
#include <QPixmap>
#include "Application.h"
#include "FileTable.h"
#include "MainWindow.h"
#include "greucore/include/Log.h"
#include "greucore/include/Saver.h"
#include "greucore/include/StandardPaths.h"
#include "greucore/include/utils.h"
#include "greuid3/include/FileCreator.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/AudioCollection.h"
#include "greutag/include/File.h"
#include "greutag/include/FileCreatorRegistry.h"
#include "greutag/include/Tag.h"
#include "greuui/include/SplashScreen.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct Dispatcher::Detail {
    QString audioPath;
    uint packageSize = 200, messageSize = 50;
    bool noSplash = false;
    Log log;
    id3::FileCreator id3Creator;
    unique_ptr<AudioCollection> audioCollection;
    grylizo::MainWindow mainWindow;
    unique_ptr<Saver> saver;
    ScopedConnection connMod;

    Detail() { audioPath = StandardPaths::writeableLocation(QStandardPaths::MusicLocation); }

    void initialize(Dispatcher *that) {
        Modifiable::enableTracking(false);
        audioCollection = make_unique<AudioCollection>(packageSize, messageSize);
        QPixmap pixmap(":/images/images/splash.png");
        QElapsedTimer time;
        time.start();
        {
            ScopedConnection ctcann;
            SplashScreen *splash;
            boost::signals2::connection ctconn;
            FileCreatorRegistry::instance().registerAddon(&id3Creator, "id3");
            if (!noSplash) {
                splash = new SplashScreen(pixmap);
                splash->setAutoFillBackground(true);
                splash->show();
                ctcann = audioCollection->counted().connect(
                    [&splash](size_t percent) { splash->setPercent(percent); });
            }
            audioCollection->initialize(audioPath, &log);
            if (!noSplash) {
                splash->setPercent(100);
                splash->finish(&mainWindow);
                ctconn.disconnect();
            }
        }
        log.addInfo(QObject::tr("Initialization time: %1 ms").arg(time.elapsed()));
        mainWindow.setupUi();

        Modifiable::enableTracking(true);
        connMod = audioCollection->modifiedChanged().connect(
            [that](auto v) { that->setModified(v); });
        mainWindow.show();
    }
};

Dispatcher::Dispatcher()
    : d(GREU_MKDET()) {}

void Dispatcher::setAudioPath(QString const &v) {
    d->audioPath = v;
}

void Dispatcher::setPackageSize(uint v) {
    d->packageSize = v;
}

void Dispatcher::setMessageSize(uint v) {
    d->messageSize = v;
}

void Dispatcher::setNoSplash(bool v) {
    d->noSplash = v;
}

void Dispatcher::initialize() {
    d->initialize(this);
}

ui::MainWindow *Dispatcher::mainWindow() const {
    return &d->mainWindow;
}

size_t Dispatcher::modifiedCount() const {
    return d->audioCollection->modifiedFilesCount();
}

size_t Dispatcher::savedCount() const {
    return d->audioCollection->savedFilesCount();
}

AudioCollection *Dispatcher::audioCollection() const {
    return d->audioCollection.get();
}

void Dispatcher::tagSelectedFiles(Attribute *att) const {
    auto selected = safeCast<grylizo::MainWindow>(mainWindow())->fileTable()->selectedFiles();
    auto current = att->tag()->file();
    for (auto f : selected)
        if (f != current) {
            auto to = f->tag()->attribute(att->type()->id());
            if (to)
                to->copyValue(*att);
        }
}

void Dispatcher::tagContainer(tag::Attribute *att) const {
    auto container = dynamic_cast<grylizo::MainWindow *>(mainWindow())->fileTable()->fileContainer();
    if (!container)
        return;
    auto current = att->tag()->file();
    container->forEach([current, att](auto f) {
        if (f != current) {
            auto to = f->tag()->attribute(att->type()->id());
            if (to)
                to->copyValue(*att);
        }
    });
}

QString Dispatcher::audioPath() const {
    return d->audioPath;
}

void Dispatcher::saveOtherSettings() {
    d->mainWindow.saveOtherSettings();
}

void Dispatcher::restoreOtherSettings() {
    d->mainWindow.restoreOtherSettings();
}

void Dispatcher::updateUi() {
    d->mainWindow.update();
}

Saver *Dispatcher::saver() const {
    d->saver = d->audioCollection->saver();
    return d->saver.get();
}

FileContainer *Dispatcher::fileContainer() const {
    return safeCast<grylizo::MainWindow>(mainWindow())->fileTable()->fileContainer();
}

Log &Dispatcher::log() const {
    return d->log;
}

} // namespace greu::grylizo
