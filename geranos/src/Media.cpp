#include "Media.h"
#include <QDir>
#include <QFileInfo>
#include <QMessageBox>
#include <QProcess>
#include "greucore/include/Language.h"
#include "greucore/include/hexUtils.h"
#include "greucore/include/stringUtils.h"

using namespace std;

namespace greu::geranos {

using namespace core;

struct Media::Detail {
    struct Parser {
        Tracks *videoTracks = nullptr, *audioTracks = nullptr, *subtitleTracks = nullptr;
        vector<QString> strings;

        Track current;

        Parser(QString const &string)
            : strings(splitLines(string)) {}

        void parse() {
            for (auto &s : strings) {
                if (s == "[STREAM]") {
                    addTrack();
                } else if (s.contains("=")) {
                    auto words = s.split("=", Qt::SkipEmptyParts);
                    if (words.size() > 1) {
                        auto &key = words[0];
                        auto &value = words[1];
                        if (key == "codec_type") {
                            if (value == "video")
                                current.type = Video;
                            else if (value == "audio")
                                current.type = Audio;
                            else if (value == "subtitle")
                                current.type = Subtitle;
                            else
                                current.type = None;
                        } else if (key == "id") {
                            current.id = convertHexString<uint>(words[1].sliced(2)) % 256;
                        } else if (key == "duration") {
                            auto dur = value.toDouble();
                            current.duration = dur;
                        } else if (key == "TAG:language") {
                            if (value != "und") {
                                auto o = Language::fromCode2(value);
                                if (o) {
                                    current.languageCode2 = o.value()->code2();
                                    current.language = o.value()->name();
                                } else {
                                    current.language = "undefined";
                                }
                            }
                        } else if (key == "channels") {
                            current.channels = value.toUInt();
                        } else if (key == "codec_name") {
                            current.codecName = value;
                        } else if (key == "channel_layout") {
                            current.channelLayout = value;
                        }
                    }
                }
            }
            addTrack();
        }

        void addTrack() {
            switch (current.type) {
            case None:
                break;
            case Video:
                current.number = videoTracks->size() + 1;
                videoTracks->push_back(current);
                break;
            case Audio:
                current.number = audioTracks->size() + 1;
                audioTracks->push_back(current);
                break;
            case Subtitle:
                current.number = subtitleTracks->size() + 1;
                subtitleTracks->push_back(current);
                break;
            default:
                break;
            }
            current = Track{};
        }
    };

    static QString const ffmpegCommand, ffprobeCommand, lsdvdCommand;

    vector<QString> fileNames;
    QString path;
    bool parsed = false;
    Tracks videoTracks, audioTracks, subtitleTracks;

    bool valid = false, hasError = false;
    QString stdOut{""}, errorText{""}, lastCommand{""};

    static vector<QString> setFileNames(vector<QString> const &filePaths) {
        decltype(setFileNames({})) res;
        assert(filePaths.size() > 0 && "No file names for Media");
        auto path = QFileInfo{filePaths[0]}.path();
        for (auto &f : filePaths) {
            auto fi = QFileInfo{f};
            assert(path == fi.path() && "Files of media do not belong to one directory");
            res.push_back(fi.fileName());
        }
        return res;
    }

    Detail(vector<QString> const &filePaths)
        : fileNames(setFileNames(filePaths))
        , path(QFileInfo{filePaths[0]}.path()) {}

    void parse() {
        if (parsed)
            return;
        parsed = true;
        valid = false;
        videoTracks.clear();
        audioTracks.clear();
        subtitleTracks.clear();
        auto args = fillArgs();
        args.push_back("-show_streams");
        submitProgram(ffprobeCommand, args);
        if (stdOut.startsWith("[STREAM]")) {
            hasError = false;
            errorText.clear();
        }
        if (hasError)
            return;
        Parser parser{stdOut};
        parser.videoTracks = &videoTracks;
        parser.audioTracks = &audioTracks;
        parser.subtitleTracks = &subtitleTracks;
        parser.parse();
        if (fileNames.size() > 1) {
            uint n = subtitleTracks.size() + 1;
            for (auto &f : fileNames) {
                if (isSubtitleFile(f)) {
                    subtitleTracks.push_back(Track{.number = n,
                                                   .type = Subtitle,
                                                   .id = 0,
                                                   .codecName = "srt",
                                                   .fileName = f});
                    ++n;
                }
            }
        }
        if (videoTracks.size() > 0 || subtitleTracks.size() > 0)
            valid = true;
    }

    void submitProgram(QString const &program, vector<QString> &args) {
        hasError = false;
        errorText = "";
        stdOut = "";
        lastCommand = program;
        for (auto &s : args) {
            lastCommand.append(" ").append(s);
        }
        QProcess p;
        p.setWorkingDirectory(path);
        p.start(program, ranges::to<QStringList>(args));
        p.waitForFinished(-1);
        errorText = QString(p.readAllStandardError());
        stdOut = QString(p.readAllStandardOutput());
        if (p.exitStatus() == QProcess::CrashExit)
            hasError = true;
        if (p.exitCode() != 0)
            hasError = true;
    }

    // ffmpeg -i video.mp4 -i image.png -map 1 -map 0 -c copy -disposition:0 attached_pic out.mp4

    vector<QString> fillArgs(QString const &from = "", QString const &to = "") {
        decltype(fillArgs()) res{
            {"-hide_banner"},
            {"-probesize"},
            {QString{"%1M"}.arg(LookAhead)},
            {"-analyzeduration"},
            {QString{"%1M"}.arg(LookAhead)},
        };
        if (!from.isEmpty()) {
            res.push_back("-ss");
            res.push_back(from);
        }
        if (!to.isEmpty()) {
            res.push_back("-to");
            res.push_back(to);
        }
        res.push_back("-i");
        if (fileNames.size() == 1) {
            res.push_back(fileNames[0]);
            res.push_back("-movflags");
            res.push_back("use_metadata_tags");
        } else {
            QString in{"concat:"};
            auto first = true;
            for (auto &f : fileNames) {
                if (!isSubtitleFile(f)) {
                    if (first) {
                        in += escapeFileName(f);
                        first = false;
                    } else {
                        in += "|" + escapeFileName(f);
                    }
                }
            }
            res.push_back(in);
        }
        return res;
    }

    bool isSubtitleFile(QString const &fileName) {
        return QFileInfo{fileName}.suffix().toUpper() == "SRT";
    }

    void convert(TrackConversions const &audio,
                 TrackConversions const &subtitle,
                 QString const &from,
                 QString const &to,
                 bool useVob) {
        auto args = fillArgs(from, to);
        args.push_back("-map");
        args.push_back("0:v?");
        addTracks(audio, "a", args, true);
        addTracks(subtitle, "s", args);
        args.push_back("-vcodec");
        // if(useVob)
        //     args.push_back("mpeg2video");
        // else
        //     args.push_back("copy");
        args.push_back("copy");
        args.push_back("-acodec");
        args.push_back("copy");
        bool hasDts = false;
        setAudioCodecs(audio, args, hasDts);
        args.push_back("-scodec");
        args.push_back("copy");
        if (hasDts) {
            args.push_back("-strict");
            args.push_back("-2");
        }
        // args.push_back("video_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + ".MP4");
        args.push_back("video_" + QDateTime::currentDateTime().toString("yyyyMMddhhmmss") + "."
                       + (useVob ? "VOB" : "MP4"));
        submitProgram(ffmpegCommand, args);
        /*
         * /usr/bin/ffmpeg -hide_banner -probesize 10000M -analyzeduration 10000M -i video.VOB -map 0:v? -map 0:a:0 -disposition:a:0 default -metadata:s:a:0 language=deu -metadata:s:a:0 title=5.1 -map 0:a:1 -metadata:s:a:1 language=eng -metadata:s:a:1 title=5.1 -map 0:a:2 -metadata:s:a:2 language=deu -metadata:s:a:2 title=DTS -map 0:a:3 -metadata:s:a:3 language=eng -metadata:s:a:3 title=Commentary\ 1 -map 0:a:4 -metadata:s:a:4 language=eng -metadata:s:a:4 title=Commentary\ 2 -map 0:s:4 -disposition:s:0 default -metadata:s:s:0 language=eng -map 0:s:2 -metadata:s:s:1 language=deu -map 0:s:0 -metadata:s:s:2 language=deu -metadata:s:s:2 title=Kommentar\ 1 -map 0:s:1 -metadata:s:s:3 language=deu -metadata:s:s:3 title=Kommentar\ 2 -strict -2 -vcodec copy -acodec ac3 -codec:a:2 dts -scodec copy video1.MP4
-----------------------------------------
         */
    }

    void setAudioCodecs(TrackConversions const &tracks, vector<QString> &args, bool &hasDts) {
        hasDts = false;
        for (auto &t : tracks) {
            if (t.audioCodec != Default) {
                args.push_back("-codec:a:" + QString::number(t.numberNew - 1));
                args.push_back(t.audioCodec == Dts   ? "dts"
                               : t.audioCodec == Pcm ? "pcm_s16be"
                               : t.audioCodec == Aac ? "aac"
                                                     : "ac3");
                if (!hasDts)
                    hasDts = t.audioCodec == Dts;
            }
        }
    }

    void addTracks(TrackConversions const &tracks,
                   QString const &trackChar,
                   vector<QString> &args,
                   bool setFirstAsDefault = false) {
        auto first = true;
        for (auto &t : tracks) {
            auto no = QString::number(t.numberOld - 1);
            auto nn = QString::number(t.numberNew - 1);
            args.push_back("-map");
            args.push_back("0:" + trackChar + ":" + no);
            if (first && setFirstAsDefault) {
                args.push_back("-disposition:" + trackChar + ":" + nn);
                args.push_back("default");
                first = false;
            }
            if (!t.language.isEmpty()) {
                auto o = Language::fromName(t.language);
                if (o) {
                    args.push_back("-metadata:s:" + trackChar + ":" + nn);
                    args.push_back("language=" + o.value()->code2());
                }
            }
            if (!t.title.isEmpty()) {
                args.push_back("-metadata:s:" + trackChar + ":" + nn);
                args.push_back("title=" + t.title);
            }
        }
    }
};

QString const Media::Detail::ffmpegCommand{"/usr/bin/ffmpeg"},
    Media::Detail::ffprobeCommand{"/usr/bin/ffprobe"},
    Media::Detail::lsdvdCommand{"/usr/bin/lsdvd"};

Media::Media(vector<QString> const &filePaths)
    : d(GREU_MKDET(filePaths)) {}

Media::Media(Media const &other)
    : d(GREU_MKDET(*other.d)) {}

Media &Media::operator=(Media const &other) {
    *d = *other.d;
    return *this;
}

Media::Tracks const &Media::videoTracks() const {
    d->parse();
    return d->videoTracks;
}

Media::Tracks const &Media::audioTracks() const {
    d->parse();
    return d->audioTracks;
}

Media::Tracks const &Media::subtitleTracks() const {
    d->parse();
    return d->subtitleTracks;
}

bool Media::isValid() const {
    d->parse();
    return d->valid;
}

QString Media::filePath() const {
    return QDir{d->path}.filePath(d->fileNames[0]);
}

QString Media::fileName() const {
    return d->fileNames[0];
}

uint Media::videoDuration() const {
    if (d->videoTracks.size() == 0)
        return 0;
    return d->videoTracks[0].duration;
}

QString Media::path() const {
    return d->path;
}

void Media::convert(TrackConversions const &audio,
                    TrackConversions const &subtitle,
                    QString const &from,
                    QString const &to,
                    bool useVob) {
    d->convert(audio, subtitle, from, to, useVob);
}

bool Media::hasError() const {
    return d->hasError;
}

QString Media::errorText() const {
    return d->errorText;
}

QString Media::lastCommand() const {
    return d->lastCommand;
}

} // namespace greu::geranos
