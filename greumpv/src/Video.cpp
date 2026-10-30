#include "Video.h"
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QProcess>
#include "greucore/include/Language.h"
#include "greucore/include/jsonUtils.h"

using namespace std;

namespace greu::mpv {

using namespace core;

struct Video::Detail {
    QFileInfo fileInfo;

    uint videoCount, audioCount, textCount, menuCount;
    uint durationSecs;
    vector<VideoTrack> videoTracks;
    vector<AudioTrack> audioTracks;
    vector<TextTrack> textTracks;
    uint videoId = 1, audioId = 1, textId = 1;

    Detail(QFileInfo const &fi)
        : fileInfo(fi) {}

    void analyze() {
        QProcess p;
        p.start("mediainfo", {"--Output=JSON", fileInfo.absoluteFilePath()});
        p.waitForFinished();
        parse(QJsonDocument::fromJson(p.readAllStandardOutput()).object());
    }

    static bool less(Track const &t1, Track const &t2) { return t1.id < t2.id; }

    void parse(auto const &o) {
        auto media = o.value("media").toObject();
        auto tracks = media.value("track").toArray();
        for (auto t : tracks) {
            auto track = t.toObject();
            auto type = valueString(track, "@type");
            if (type == "General")
                parseGeneral(track);
            else if (type == "Video")
                videoTracks.push_back(parseVideo(track));
            else if (type == "Audio")
                audioTracks.push_back(parseAudio(track));
            else if (type == "Text")
                textTracks.push_back(parseText(track));
            else if (type == "Menu")
                parseMenu(track);
        }
        ranges::sort(videoTracks, less);
        ranges::sort(audioTracks, less);
        ranges::sort(textTracks, less);
    }

    void parseGeneral(QJsonObject const &json) {
        videoCount = valueUIntS(json, "VideoCount");
        audioCount = valueUIntS(json, "AudioCount");
        menuCount = valueUIntS(json, "MenuCount");
        textCount = valueUIntS(json, "TextCount");
        durationSecs = (uint) valueDoubleS(json, "Duration");
    }

    VideoTrack parseVideo(QJsonObject const &json) {
        VideoTrack res;
        res.type = TrackType::video;
        parseTrack(json, &res, videoId);
        res.aspectRatio = valueDoubleS(json, "DisplayAspectRatio");
        return res;
    }

    AudioTrack parseAudio(QJsonObject const &json) {
        AudioTrack res;
        res.type = TrackType::audio;
        parseTrack(json, &res, audioId);
        res.channels = valueUIntS(json, "Channels");
        res.format = valueString(json, "Format");
        res.isDefault = valueBool(json, "Default");
        return res;
    }

    TextTrack parseText(QJsonObject const &json) {
        TextTrack res;
        res.type = TrackType::text;
        parseTrack(json, &res, textId);
        res.format = valueString(json, "Format");
        res.isDefault = valueBool(json, "Default");
        res.isForced = valueBool(json, "Forced");
        return res;
    }

    void parseTrack(QJsonObject const &json, Track *track, uint &id) {
        track->id = id++;
        track->durationSecs = valueDoubleS(json, "Duration");
        track->title = valueString(json, "Title");
        auto lang = valueString(json, "Language");
        if (lang.length() == Language::code1Len) {
            auto lc = Language::fromCode1(lang);
            if (lc)
                track->language = lc.value()->name();
        } else if (lang.length() == Language::code2Len) {
            auto lc = Language::fromCode2(lang);
            if (lc)
                track->language = lc.value()->name();
        } else
            track->language = lang;
    }

    void parseMenu(QJsonObject const &json) {
        if (valueString(json, "Format") != "Timed Text")
            return;
        auto m = json.value("extra").toObject();
        auto mf = valueString(m, "Menu_For");
        VideoTrack *vt = nullptr;
        for (auto &iv : mf.split(",", Qt::SkipEmptyParts))
            if (auto v = ranges::find(videoTracks, iv.toUInt(), &VideoTrack::id);
                v != videoTracks.end())
                vt = &(*v);
        if (!vt)
            return;
        for (auto k : m.keys()) {
            if (!k.startsWith("_"))
                continue;
            auto ts = k.split("_", Qt::SkipEmptyParts);
            if (ts.size() != 4)
                continue;
            QTime t{ts[0].toInt(), ts[1].toInt(), ts[2].toInt(), ts[3].toInt()};
            vt->chapters.push_back(
                {QTime(ts[0].toInt(), ts[1].toInt(), ts[2].toInt(), ts[3].toInt()),
                 valueString(m, k)});
        }
        ranges::sort(vt->chapters, [](auto fst, auto snd) { return fst.time < snd.time; });
    }
};

Video::Video(QFileInfo const &fi)
    : d(GREU_MKDET(fi)) {
    d->analyze();
}

uint Video::durationSecs() const {
    return d->durationSecs;
}
Video::VideoTracks const &Video::videoTracks() const {
    return d->videoTracks;
}
Video::AudioTracks const &Video::audioTracks() const {
    return d->audioTracks;
}
Video::TextTracks const &Video::textTracks() const {
    return d->textTracks;
}

} // namespace greu::mpv
