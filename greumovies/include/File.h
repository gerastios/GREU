#pragma once
#include <QDate>
#include "Object.h"
#include "greucore/include/DetailPtr.h"
#include "greumpv/include/Video.h"
#include "property.h"

class QFileInfo;

namespace greu::movies {

class Settings;
struct Video;
class Movie;
class Episode;

class GREU_PUBLIC(MOVIES) File : public Object {
public:
    enum class Type : int {
        none = 0,
        movie = 1,
        episode = 2,
    };

    static File *file(core::Uuid const &);

    virtual void read() override;
    virtual void save() override;
    void saveLastViewed();

    QFileInfo fileInfo() const;
    Type mediumType() const;
    Video *video() const;
    void setVideo(Video *);

    movies::Movie *movie() const;
    movies::Episode *episode() const;

    bool operator==(File const &) const;
    inline bool operator!=(File const &other) const { return !(*this == other); }

    bool isUsed() const;
    static bool isUsed(QString const &absoluteName);
    bool isValid() const;
    static bool isValid(QString const &absoluteName);
    QString relativeName() const;
    static QString relativeName(QString const &absoluteName);
    static QString absoluteName(QString const &relativeName);

    GREU_P_DEF(runtimeSecs, RuntimeSecs, uint)
    GREU_P_DEF(lastPosition, LastPosition, double)
    GREU_P_DEF(lastAudioTrack, LastAudioTrack, uint)
    GREU_P_DEF(lastTextTrack, LastTextTrack, uint)
    GREU_P_DEF(lastViewed, LastViewed, QDateTime)

    QString percentViewed() const;

private:
    friend class FilesTable;
    friend class Object;
    File(core::Uuid const &);
    void setFileInfo(QFileInfo const &);
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies
