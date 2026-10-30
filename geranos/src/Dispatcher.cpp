#include "Dispatcher.h"
#include <QFileInfo>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QProcess>
#include <QToolBar>
#include "ChapterEditor.h"
#include "ConvertDialog.h"
#include "FileListTable.h"
#include "MainWindow.h"
#include "TimeCalculator.h"
#include "TrackNumberDialog.h"
#include "WorkFileTable.h"
#include "dvdInfo.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/StandardPaths.h"
#include "greuui/include/Action.h"
#include "greuui/include/Icons.h"
#include "greuui/include/Notepad.h"
#include "greuui/include/connect.h"
#include <vector>

using namespace std;

namespace {
vector<QString> mappings{"-map",
                         "0:v?",
                         "-map",
                         "0:a?",
                         "-map",
                         "0:s?",
                         "-vcodec",
                         "copy",
                         "-acodec",
                         "copy",
                         "-scodec",
                         "copy"};
}

namespace greu::geranos {

using namespace core;
using namespace ui;

struct Dispatcher::Detail {
    GuardedPtr<MainWindow> mainWindow;
    QString path;
    GuardedPtr<QMenuBar> menu;
    GuardedPtr<QMenu> fileMenu, utilityMenu;
    GuardedPtr<QToolBar> toolbar;
    GuardedPtr<Action> actConvert, actClear, actTimeCalc, actTrackInfo, actChaptersOfTrack;
    GuardedPtr<ConvertDialog> convertDialog;
    GuardedPtr<Notepad> notepad;
    GuardedPtr<TimeCalculator> timeCalculator;
    ScopedConnection connExternPlayRequested, connTrackInfoRequested, connBackupRequested,
        connTrackChapters, connEditChapters;

    void initialize() {
        path = StandardPaths::writeableLocation(QStandardPaths::MoviesLocation);
        mainWindow = new MainWindow();
        mainWindow->setupUi();

        menu = new QMenuBar;
        toolbar = new QToolBar;
        toolbar->setObjectName("mainToolbar");
        fileMenu = new QMenu("&File");
        menu->addMenu(fileMenu.get());
        utilityMenu = new QMenu("&Utilities");
        menu->addMenu(utilityMenu.get());
        mainWindow->setMenuBar(menu.get());

        actConvert = new Action(GREU_ICON(convert), QObject::tr("&Convert ..."));
        ui::connect(actConvert.get(), [this] {
            auto media = mainWindow->fileTable()->media();
            if (media) {
                if (convertDialog)
                    convertDialog->hide();
                convertDialog.reset(new ConvertDialog{std::move(media.value())});
                convertDialog->show();
                convertDialog->raise();
                convertDialog->activateWindow();
            }
        });

        actClear = new Action(GREU_ICON(erase), QObject::tr("C&lear files ..."));
        ui::connect(actClear.get(), [this] { mainWindow->fileTable()->clear(); });

        actTimeCalc = new Action(QObject::tr("Calculate times"));
        ui::connect(actTimeCalc.get(), [this] {
            if (timeCalculator)
                timeCalculator->hide();
            timeCalculator.reset(new TimeCalculator);
            timeCalculator->show();
            timeCalculator->raise();
            timeCalculator->activateWindow();
        });

        actTrackInfo = new Action(QObject::tr("Show tracks for device"));
        ui::connect(actTrackInfo.get(), [this] {
            TrackNumberDialog d{TrackNumberDialog::Device};
            d.setupUi();
            if (d.exec())
                showTracks(d.device(), d.device());
        });

        actChaptersOfTrack = new Action(QObject::tr("Show chapters of track for device"));
        ui::connect(actChaptersOfTrack.get(), [this] {
            TrackNumberDialog d{TrackNumberDialog::TrackNumber | TrackNumberDialog::Device};
            d.setupUi();
            if (d.exec())
                showChaptersOfTrack(d.device(), d.trackNumber());
        });

        fileMenu->addAction(actConvert.get());
        fileMenu->addAction(actClear.get());
        utilityMenu->addAction(actTimeCalc.get());
        utilityMenu->addAction(actTrackInfo.get());
        utilityMenu->addAction(actChaptersOfTrack.get());
        toolbar->addAction(actConvert.get());
        toolbar->addAction(actClear.get());
        mainWindow->addToolBar(toolbar.get());

        connExternPlayRequested = mainWindow->fileList()->externPlayRequested().connect(
            [](auto filename) { QProcess::startDetached("vlc", {filename}); });
        connTrackInfoRequested = mainWindow->fileList()->trackInfoRequested().connect(
            [this](auto media) { showTracks(media.filePath(), media.fileName()); });
        connBackupRequested = mainWindow->fileList()->backupRequested().connect(
            [this](auto filename) {
                QFileInfo f{{filename}};
                TrackNumberDialog d{TrackNumberDialog::TrackNumber};
                d.setupUi();
                if (d.exec()) {
                    QProcess p;
                    p.setWorkingDirectory(f.path());
                    vector<QString> args{
                        "-t",
                        QString::number(d.trackNumber()),
                        "-i",
                        f.fileName(),
                    };
                    p.start("/usr/bin/dvdbackup", ranges::to<QStringList>(args));
                    p.waitForFinished(-1);
                    auto err = QString(p.readAllStandardError());
                    if (!err.isEmpty()) {
                        QMessageBox::critical(mainWindow.get(), QObject::tr("Backup failed"), err);
                    }
                }
            });
        connTrackChapters = mainWindow->fileList()->trackChapters().connect([this](auto filename) {
            QFileInfo f{{filename}};
            TrackNumberDialog d{TrackNumberDialog::TrackNumber};
            d.setupUi();
            if (d.exec())
                showChaptersOfTrack(f.path(), d.trackNumber(), f.fileName());
        });
        connEditChapters = mainWindow->fileList()->editChapters().connect(
            [this](auto filename) { editChapters(filename); });

        mainWindow->show();
    }

    void editChapters(QString const &fileName) {
        ChapterEditor ed{fileName};
        if (ed.exec()) {
            auto chapters = ed.chapters();
            auto time = QDateTime::currentDateTime().toString("yyyyMMddhhmmss");
            QFileInfo fi{{fileName}};
            auto noChapter = fi.completeBaseName() + time + "_no_chapters.MP4";
            auto meta = fi.completeBaseName() + time + "_meta.txt";
            auto final = fi.completeBaseName() + time + ".MP4";
            auto path = fi.path();
            processFile(path, noChapter, {"-i", fi.fileName(), "-map_chapters", "-1"});
            processFile(path, meta, {"-i", noChapter, "-f", "ffmetadata"}, false);
            QFile metaFile{path + "/" + meta};
            if (!metaFile.exists()) {
                QMessageBox::critical(mainWindow.get(),
                                      QObject::tr("File not found"),
                                      QObject::tr("File %1 does not exist").arg(meta));
                QFile::remove(noChapter);
                QFile::remove(meta);
                return;
            } else {
                if (!metaFile.open(QFile::Append)) {
                    QMessageBox::critical(mainWindow.get(),
                                          QObject::tr("File open error"),
                                          QObject::tr("File %1 could not be opened: %2")
                                              .arg(meta, metaFile.errorString()));
                    QFile::remove(noChapter);
                    QFile::remove(meta);
                    return;
                } else {
                    metaFile.write(chapters.toStdString().c_str());
                    metaFile.close();
                }
            }
            processFile(path, final, {"-i", noChapter, "-i", meta, "-map_metadata", "1"});
            QFile::remove(path + "/" + noChapter);
            QFile::remove(path + "/" + meta);
        }
    }

    void showTracks(QString const &path, QString const &title) {
        if (notepad)
            notepad->hide();
        notepad.reset(new Notepad("trackInfoNotepad"));
        notepad->setupUi();
        notepad->setText(dvdInfo(path));
        notepad->setWindowTitle(title);
        notepad->show();
        notepad->raise();
        notepad->activateWindow();
    }

    void showChaptersOfTrack(QString const &path, int trackNumber, QString const &fileName = {}) {
        QProcess p;
        vector<QString> args;
        if (fileName.isEmpty()) {
            args = {
                "-t",
                QString::number(trackNumber),
                "-c",
                path,
            };
        } else {
            p.setWorkingDirectory(path);
            args = {
                "-t",
                QString::number(trackNumber),
                "-c",
                fileName,
            };
        }
        p.start("/usr/bin/lsdvd", ranges::to<QStringList>(args));
        p.waitForFinished(-1);
        auto err = QString(p.readAllStandardError());
        if (!err.isEmpty()) {
            QMessageBox::critical(mainWindow.get(), QObject::tr("Backup failed"), err);
        }
        auto lines = QString(p.readAllStandardOutput())
                         .split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);
        QString chapters;
        QTime t{0, 0};
        QTime dur;
        for (auto &s : lines) {
            auto s1 = s.simplified();
            if (s1.startsWith("Chapter")) {
                auto words = s1.split(' ', Qt::SkipEmptyParts);
                if (words.size() >= 4) {
                    auto s = words[3].simplified();
                    s.replace(',', ' ');
                    s = s.simplified();
                    dur = QTime::fromString(s, "h:mm:ss.zzz");
                    chapters += QObject::tr("%1\n").arg(t.toString("h:mm:ss.zzz"));
                    t = t.addMSecs(dur.msecsSinceStartOfDay());
                }
            }
        }
        if (notepad)
            notepad->hide();
        notepad.reset(new Notepad("trackChapterNotepad"));
        notepad->setupUi();
        notepad->setText(chapters);
        notepad->setWindowTitle(fileName.isEmpty() ? path : fileName);
        notepad->show();
        notepad->raise();
        notepad->activateWindow();
    }

    void processFile(QString const &path,
                     QString const &targetFileName,
                     vector<QString> args,
                     bool withMappings = true) {
        QProcess p;
        p.setWorkingDirectory(path);
        if (withMappings)
            args.append_range(mappings);
        args.push_back(targetFileName);
        p.start("/usr/bin/ffmpeg", ranges::to<QStringList>(args));
        p.waitForFinished(-1);
    }
};

Dispatcher::Dispatcher()
    : ui::Dispatcher()
    , d(GREU_MKDET()) {}

QString Dispatcher::path() const {
    return d->path;
}

void Dispatcher::initialize() {
    d->initialize();
}

ui::MainWindow *Dispatcher::mainWindow() const {
    return d->mainWindow.get();
}

} // namespace greu::geranos
