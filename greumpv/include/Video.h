#pragma once
#include <QString>
#include <QTime>
#include "greucore/include/DetailPtr.h"

class QFileInfo;

namespace greu::mpv {

class GREU_PUBLIC(MPV) Video {
public:
    Video(QFileInfo const &);

    enum class TrackType {
        video,
        audio,
        text,
        menu,
    };

    struct Track {
        uint id;
        TrackType type;
        double durationSecs;
        QString title, language;
    };

    struct Chapter {
        QTime time;
        QString title;
    };

    struct VideoTrack : public Track {
        float aspectRatio;
        std::vector<Chapter> chapters;
    };

    struct AudioTrack : public Track {
        uint channels;
        QString format;
        bool isDefault;
    };

    struct TextTrack : public Track {
        QString format;
        bool isDefault, isForced;
    };

    using VideoTracks = std::vector<VideoTrack>;
    using AudioTracks = std::vector<AudioTrack>;
    using TextTracks = std::vector<TextTrack>;

    uint durationSecs() const;
    VideoTracks const &videoTracks() const;
    AudioTracks const &audioTracks() const;
    TextTracks const &textTracks() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::mpv
