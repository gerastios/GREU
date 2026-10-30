#pragma once

#include <QString>
#include "greucore/include/DetailPtr.h"
#include <vector>

namespace greu::geranos {

class Media {
public:
    static uint const LookAhead = 10000;

    enum TrackType {
        None,
        Video,
        Audio,
        Subtitle,
    };

    enum AudioCodec {
        Default,
        Ac3,
        Aac,
        Dts,
        Pcm,
    };

    struct Track {
        uint number{0};
        TrackType type{None};
        uint id{0}, duration{0};
        QString languageCode2{""}, language{""}, codecName{""};
        uint channels{0};
        QString channelLayout{""}, fileName{""};
    };

    struct TrackConversion {
        uint numberOld{0}, numberNew{0};
        QString languageCode2{""}, language{""}, title{""};
        AudioCodec audioCodec{Default};
        QString offset{""};
    };

    using Tracks = std::vector<Track>;
    using TrackConversions = std::vector<TrackConversion>;

    Media(std::vector<QString> const &filePaths);
    Media(Media const &);
    Media &operator=(Media const &);
    GREU_MOVE(Media);

    Tracks const &videoTracks() const;
    Tracks const &audioTracks() const;
    Tracks const &subtitleTracks() const;
    bool isValid() const;

    QString filePath() const;
    QString fileName() const;
    uint videoDuration() const;
    QString path() const;

    void convert(TrackConversions const &audio,
                 TrackConversions const &subtitle,
                 QString const &from = "",
                 QString const &to = "",
                 bool useVob = false);

    [[nodiscard]] bool hasError() const;
    [[nodiscard]] QString errorText() const;
    [[nodiscard]] QString lastCommand() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::geranos
