#include "Player.h"
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QToolBar>
#include "greucore/include/GuardedPtr.h"
#include "greumovies/include/File.h"
#include "greumovies/include/Settings.h"
#include "greumpv/include/Player.h"
#include "greumpv/include/Video.h"
#include "greuui/include/Application.h"
#include "greuui/include/ComboBox.h"
#include "greuui/include/Dialog.h"
#include "greuui/include/Icons.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"
#include <thread>

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;
using namespace ui;

struct Player::Detail {
    struct Selection : public Dialog {
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<QLabel> audioLabel, subtitleLabel, chapterLabel;
        GuardedPtr<ComboBox> audio, subtitle, chapter;
        unique_ptr<mpv::Video> video;
        vector<mpv::Video::AudioTrack> const *audioTracks;
        vector<mpv::Video::TextTrack> const *textTracks;
        GuardedPtr<QToolBar> toolBar;
        GuardedPtr<QPushButton> ok, cancel;

        Selection(File *file) {
            layout = new QGridLayout;
            setLayout(layout.get());
            addNewWidget(ok, GREU_ICON(ok), "", toolBar.get());
            addNewWidget(cancel, GREU_ICON(cancel), "", toolBar.get());
            ok->setDefault(true);
            ui::connect(ok.get(), [this] { accept(); });
            ui::connect(cancel.get(), [this] { reject(); });
            addNewWidget(audioLabel, QObject::tr("Audio track"), layout.get(), 0, 0);
            addNewWidget(audio, layout.get(), 0, 1);
            audio->setEditable(false);
            addNewWidget(subtitleLabel, QObject::tr("Subtitle track"), layout.get(), 1, 0);
            addNewWidget(subtitle, layout.get(), 1, 1);
            subtitle->setEditable(false);
            addNewWidget(chapterLabel, QObject::tr("Chapter"), layout.get(), 2, 0);
            addNewWidget(chapter, layout.get(), 2, 1);
            chapter->setEditable(false);
            addNewWidget(toolBar, layout.get(), 3, 0, 1, 2);
            video = make_unique<mpv::Video>(file->fileInfo());
            audioTracks = &video->audioTracks();
            textTracks = &video->textTracks();
            for (auto t = audioTracks->cbegin(); t != audioTracks->cend(); t++)
                if (t->title.isEmpty())
                    audio->addItem(QObject::tr("%1 (channels: %2; format: %3)")
                                       .arg(t->language)
                                       .arg(t->channels)
                                       .arg(t->format),
                                   t->id);
                else
                    audio->addItem(QObject::tr("%1 - %2 (channels: %3; format: %4)")
                                       .arg(t->language)
                                       .arg(t->title)
                                       .arg(t->channels)
                                       .arg(t->format),
                                   t->id);
            subtitle->addItem(QObject::tr("none"), {});
            for (auto t = textTracks->cbegin(); t != textTracks->cend(); t++)
                if (t->title.isEmpty())
                    subtitle->addItem(QObject::tr("%12").arg(t->language), t->id);
                else
                    subtitle->addItem(QObject::tr("%1 - %2").arg(t->language).arg(t->title), t->id);
            auto vTracks = &video->videoTracks();
            auto hasChapters = false;
            if (vTracks->size() > 0)
                hasChapters = !vTracks[0].data()->chapters.empty();
            if (hasChapters) {
                auto &chapters = vTracks[0].data()->chapters;
                auto i = 0;
                for (auto t = chapters.cbegin(); t != chapters.cend(); t++)
                    chapter->addItem(QObject::tr("%1 (%2)").arg(t->title).arg(
                                         t->time.toString("h:mm:ss")),
                                     ++i);
            } else {
                chapter->setVisible(false);
                chapterLabel->setVisible(false);
            }
        }
    };

    File *file;
    unique_ptr<mpv::Player> player;
    Detail(File *f)
        : file(f) {}
    EmptySig finished;

    void play(double pos = 0, uint aid = 0, uint sid = 0, int chapter = 0) {
        if (!file)
            return;
        if (player) {
            player->stop();
            while (player)
                this_thread::sleep_for(std::chrono::milliseconds(500));
        }
        player = make_unique<mpv::Player>(file->fileInfo());
        player->finished().connect([this] {
            savePosition();
            QMetaObject::invokeMethod(
                Application::instance(), [this] { finished(); }, Qt::QueuedConnection);
            player.reset(nullptr);
        });
        player->posChanged().connect([this] { savePosition(); });
        player->play(pos, aid, sid, chapter, movies::Settings::instance()->volume());
    }

    void savePosition() {
        file->setLastPosition(player->position());
        file->setLastAudioTrack(player->aid());
        file->setLastTextTrack(player->sid());
        file->setLastViewed(QDateTime::currentDateTime());
        file->saveLastViewed();
        movies::Settings::instance()->setVolume(player->volume());
    }
};

Player::Player(File *f)
    : d(GREU_MKDET(f)) {}

void Player::play() {
    d->play();
}

void Player::playExtended() {
    Detail::Selection sel(d->file);
    if (sel.exec()) {
        uint sid = 0;
        if (sel.subtitle->currentIndex() > 0)
            sid = sel.subtitle->currentData().toUInt();
        d->play(0, sel.audio->currentData().toUInt(), sid, sel.chapter->currentData().toInt());
    }
}

void Player::playContinue() {
    d->play(d->file->lastPosition(), d->file->lastAudioTrack(), d->file->lastTextTrack());
}

EmptySig &Player::finished() const {
    return d->finished;
}

} // namespace greu::gaion
