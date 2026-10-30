#include "Player.h"
#include <QCursor>
#include <QJsonDocument>
#include <QTimer>
#include "PlayerWindow.h"
#include "greucore/include/Exception.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/signalUtils.h"
#include "mpv/client.h"

using namespace std;
using namespace std::chrono;

namespace greu::mpv {

using namespace core;

struct Player::Detail {
    GuardedPtr<PlayerWindow> window;
    QFileInfo fileInfo;
    mpv_handle *mpv = nullptr;
    int64_t mpvWid;
    EmptySig mpvEvents, finished, posChanged;
    double currentTime = -1, oldTime = 0;
    bool done = false;
    uint currAid, currSid, curVolume;
    string fileName;

    Detail(QFileInfo const &fi)
        : fileInfo(fi) {}

    void init() {
        setlocale(LC_NUMERIC, "C");
        mpv = mpv_create();
        if (!mpv)
            throw Exception("Cannot create MPV instance");
    }

    void setupUi() {
        window = new PlayerWindow;
        window->setWindowTitle(QObject::tr("MPV player"));
        window->setWindowFlags(Qt::FramelessWindowHint);
        window->setWindowState(Qt::WindowFullScreen);
        window->setAttribute(Qt::WA_DontCreateNativeAncestors);
        window->setAttribute(Qt::WA_NativeWindow);
        mpvWid = window->winId();
    }

    void play(double pos, uint aid, uint sid, int chapter, int64_t volume, Player *that) {
        setupUi();
        mpv_set_option_string(mpv, "cursor-autohide", "1000");
        mpv_set_option_string(mpv, "cursor-autohide-fs-only", "yes");
        mpv_set_option(mpv, "wid", MPV_FORMAT_INT64, &mpvWid);
        mpv_set_property_string(mpv, "input-default-bindings", "yes");
        mpv_set_property_string(mpv, "input-vo-keyboard", "yes");
        mpv_set_property_string(mpv, "cache", "yes");
        mpv_set_property_string(mpv, "osc", "yes");
        mpv_set_property_string(mpv, "ytdl", "no");
        mpv_set_property_string(mpv, "sid", sid == 0 ? "no" : QString::number(sid).toUtf8());
        mpv_set_property_string(mpv, "aid", aid == 0 ? "auto" : QString::number(aid).toUtf8());
        mpv_set_property_string(mpv, "volume", QString::number(volume).toUtf8());
        if (chapter > 0)
            mpv_set_property_string(mpv,
                                    "start",
                                    QString("#%1").arg(QString::number(chapter)).toUtf8());
        else if (pos > 0) {
            QTime zero{0, 0, 0, 0};
            auto start = zero.addMSecs(pos * 1000);
            mpv_set_property_string(mpv, "start", start.toString("HH:mm:ss.zzz").toUtf8());
        }
        mpv_observe_property(mpv, 0, "time-pos", MPV_FORMAT_DOUBLE);
        mpv_observe_property(mpv, 0, "aid", MPV_FORMAT_STRING);
        mpv_observe_property(mpv, 0, "sid", MPV_FORMAT_STRING);
        mpv_observe_property(mpv, 0, "mouse-pos", MPV_FORMAT_NODE);
        mpv_observe_property(mpv, 0, "volume", MPV_FORMAT_INT64);
        mpvEvents.connect([this] {
            while (mpv) {
                auto event = mpv_wait_event(mpv, 0);
                if (event->event_id == MPV_EVENT_NONE)
                    break;
                handleEvent(event);
            }
        });
        if (mpv_initialize(mpv) < 0)
            throw Exception("mpv failed to initialize");
        mpv_set_wakeup_callback(mpv, wakeUp, that);
        window->setModal(false);
        window->open();
        fileName = QString("%1").arg(fileInfo.absoluteFilePath()).toStdString();
        char const *args[] = {"loadfile", fileName.data(), NULL};
        mpv_command(mpv, args);
    }

    static void wakeUp(void *p) {
        auto player = (Player *) p;
        player->d->mpvEvents();
    }

    static uint toTrackNumber(char const *c) {
        if (strcmp(c, "auto") == 0)
            return 0;
        if (strcmp(c, "no") == 0)
            return 0;
        return QString(c).toUInt();
    }

    void handleEvent(mpv_event *event) {
        switch (event->event_id) {
        case MPV_EVENT_PROPERTY_CHANGE: {
            auto sendPosChanged = false;
            mpv_event_property *property = (mpv_event_property *) event->data;
            if (strcmp(property->name, "time-pos") == 0 && property->format == MPV_FORMAT_DOUBLE) {
                currentTime = *(double *) property->data;
                if (currentTime - oldTime >= 5) {
                    sendPosChanged = true;
                    oldTime = currentTime;
                }
            }
            if (strcmp(property->name, "aid") == 0 && property->format == MPV_FORMAT_STRING) {
                currAid = toTrackNumber(*(char **) (property->data));
            }
            if (strcmp(property->name, "sid") == 0 && property->format == MPV_FORMAT_STRING) {
                currSid = toTrackNumber(*(char **) (property->data));
            }
            if (strcmp(property->name, "volume") == 0) {
                curVolume = *(int64_t *) property->data;
            }
            if (strcmp(property->name, "mouse-pos") == 0)
                window->showCursor();
            if (sendPosChanged)
                posChanged();
            break;
        }
        case MPV_EVENT_END_FILE: {
            done = true;
            finished();
            break;
        }
        case MPV_EVENT_SHUTDOWN: {
            done = true;
            finished();
            break;
        }
        default:;
        }
    }

    ~Detail() {
        done = true;
        if (window)
            window.reset(nullptr);
        if (mpv)
            mpv_terminate_destroy(mpv);
    }
};

Player::Player(QFileInfo const &fi)
    : d(GREU_MKDET(fi)) {
    d->init();
}

void Player::play(double pos, uint aid, uint sid, int chapter, int64_t volume) {
    d->play(pos, aid, sid, chapter, volume, this);
}

EmptySig &Player::finished() const {
    return d->finished;
}

EmptySig &Player::posChanged() const {
    return d->posChanged;
}

void Player::stop() {
    d->done = true;
    finished();
}

double Player::position() const {
    return d->currentTime;
}
uint Player::aid() const {
    return d->currAid;
}
uint Player::sid() const {
    return d->currSid;
}
uint Player::volume() const {
    return d->curVolume;
}

} // namespace greu::mpv
