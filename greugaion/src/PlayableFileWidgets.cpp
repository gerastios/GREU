#include "PlayableFileWidgets.h"
#include <QFileInfo>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "Dispatcher.h"
#include "Styles.h"
#include "greucore/include/GuardedPtr.h"
#include "greumovies/include/File.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;
using namespace movies;

struct PlayableFileWidgets::Detail {
    GuardedPtr<QLabel> videoTracks, audioTracks, subtitleTracks, fileInfo;
    GuardedPtr<QPushButton> continuePlaying, play, playExtended;
    GuardedPtr<QHBoxLayout> buttonLayout;
    EmptySig continueRequest, playRequest, playExtendedRequest;
    ScopedConnection connFinished;

    Detail(File *file, QVBoxLayout *l) {
        if (!file)
            return;
        auto video = mpv::Video(file->fileInfo());
        auto vt = video.videoTracks();
        if (vt.size() == 1) {
            auto const &v = vt.at(0);
            if (!(v.title.isEmpty() || v.language.isEmpty()))
                addNewWidget(videoTracks,
                             GREU_STYL->tracksLabel(
                                 QObject::tr("Video track %1: %2").arg(v.title).arg(v.language)),
                             l);
        }
        auto at = video.audioTracks();
        if (at.size() > 0)
            addNewWidget(audioTracks,
                         GREU_STYL->tracksLabel(
                             QObject::tr("Audio tracks: %1").arg(buildAudioTracks(at))),
                         l);
        auto st = video.textTracks();
        if (st.size() > 0)
            addNewWidget(subtitleTracks,
                         GREU_STYL->tracksLabel(
                             QObject::tr("Subtitle tracks: %1").arg(buildSubtitleTracks(st))),
                         l);
        addNewWidget(fileInfo,
                     GREU_STYL->tracksLabel(
                         QObject::tr("File: %1").arg(file->fileInfo().absoluteFilePath())),
                     l);
        buttonLayout = new QHBoxLayout;
        l->addLayout(buttonLayout.get());
        if (file->lastPosition() > 0) {
            createContinue(file);
            buttonLayout->addWidget(continuePlaying.get());
        }
        addNewWidget(play, QObject::tr("Play"), buttonLayout.get());
        ui::connect(play.get(),
                    [file] { GREU_DC(gaion::Dispatcher)->play(file, Dispatcher::PlayType::play); });
        addNewWidget(playExtended, QObject::tr("Play..."), buttonLayout.get());
        ui::connect(playExtended.get(), [file] {
            GREU_DC(gaion::Dispatcher)->play(file, Dispatcher::PlayType::playExtended);
        });
        connFinished = GREU_DC(gaion::Dispatcher)->playingFinished().connect([this, file](File *f) {
            if (f != file)
                return;
            if (!continuePlaying) {
                createContinue(file);
                buttonLayout->insertWidget(0, continuePlaying.get());
            } else {
                continuePlaying->setText(continueText(f));
            }
        });
        buttonLayout->addStretch(1);
    }

    void createContinue(File *file) {
        continuePlaying = new QPushButton(continueText(file));
        ui::connect(continuePlaying.get(), [file] {
            GREU_DC(gaion::Dispatcher)->play(file, Dispatcher::PlayType::continuePlaying);
        });
    }

    static QString continueText(File const *file) {
        return QObject::tr("Continue (%1%%, %2)")
            .arg(file->percentViewed())
            .arg(file->lastViewed().toString("d.M.yyyy h:m"));
    }

    static QString buildAudioTracks(mpv::Video::AudioTracks const &tracks) {
        return ranges::to<QString>(
            (tracks | views::transform([](mpv::Video::AudioTrack const &track) {
                 if (track.title.isEmpty())
                     return QString("%1 (%2)").arg(track.language).arg(track.channels);
                 else
                     return QString("%1 (%2, %3)")
                         .arg(track.language)
                         .arg(track.title)
                         .arg(track.channels);
             }))
            | views::join_with(string("; ")));
    }

    static QString buildSubtitleTracks(mpv::Video::TextTracks const &tracks) {
        return ranges::to<QString>(
            (tracks | views::transform([](mpv::Video::TextTrack const &track) {
                 if (track.title.isEmpty())
                     return QString("%1").arg(track.language);
                 else
                     return QString("%1 (%2)").arg(track.language).arg(track.title);
             }))
            | views::join_with(string("; ")));
    }
};

PlayableFileWidgets::PlayableFileWidgets(File *f, QVBoxLayout *l)
    : d(GREU_MKDET(f, l)) {}

EmptySig &PlayableFileWidgets::playExtendedRequest() const {
    return d->playExtendedRequest;
}

EmptySig &PlayableFileWidgets::playRequest() const {
    return d->playRequest;
}

EmptySig &PlayableFileWidgets::continueRequest() const {
    return d->continueRequest;
}

QString runTime(int secs) {
    auto t = QTime{0, 0, 0};
    return t.addSecs(secs).toString("hh'h' mm'm'");
}

} // namespace greu::gaion
