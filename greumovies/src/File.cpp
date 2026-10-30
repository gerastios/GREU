#include "File.h"
#include <QFileInfo>
#include <QString>
#include <QVariant>
#include "Episode.h"
#include "FilesTable.h"
#include "Movie.h"
#include "Settings.h"
#include "Video.h"

using namespace std;

namespace greu::movies {

using namespace core;
using namespace movies;

struct File::Detail {
    QFileInfo fileInfo;
    Video *video = nullptr;
    GREU_P_DET(runtimeSecs, uint)
    GREU_P_DET(lastPosition, double)
    GREU_P_DET(lastAudioTrack, uint)
    GREU_P_DET(lastTextTrack, uint)
    GREU_P_DET(lastViewed, QDateTime)

    Detail()
        : runtimeSecs(0)
        , lastPosition(0)
        , lastAudioTrack(0)
        , lastTextTrack(0)
        , lastViewed{} {}
};

File *File::file(core::Uuid const &u) {
    return FilesTable::instance()->read(u);
}

void File::read() {
    FilesTable::instance()->read(this);
}

File::File(Uuid const &u)
    : Object(u)
    , d(GREU_MKDET()) {}

void File::save() {
    FilesTable::instance()->save(this);
}

void File::saveLastViewed() {
    FilesTable::instance()->saveLastViewed(this);
}

QFileInfo File::fileInfo() const {
    return d->fileInfo;
}

File::Type File::mediumType() const {
    if (!d->video)
        return Type::none;
    if (dynamic_cast<Episode *>(d->video))
        return Type::episode;
    return Type::movie;
}

bool File::isUsed() const {
    if (!isValid())
        return false;
    return FilesTable::instance()->isAssigned(relativeName()).first;
}

bool File::isUsed(QString const &absoluteName) {
    if (!isValid(absoluteName))
        return false;
    return FilesTable::instance()->isAssigned(relativeName(absoluteName)).first;
}

bool File::isValid() const {
    if (!d->fileInfo.isFile())
        return false;
    return d->fileInfo.absoluteFilePath().startsWith(Settings::instance()->pathPrefix());
}

bool File::isValid(QString const &absoluteName) {
    QFileInfo fi{absoluteName};
    if (!fi.isFile())
        return false;
    return fi.absoluteFilePath().startsWith(Settings::instance()->pathPrefix());
}

QString File::relativeName() const {
    assert(isValid());
    return d->fileInfo.absoluteFilePath().mid(Settings::instance()->pathPrefix().length());
}

QString File::relativeName(QString const &absoluteName) {
    assert(isValid(absoluteName));
    QFileInfo fi{absoluteName};
    return fi.absoluteFilePath().mid(Settings::instance()->pathPrefix().length());
}

QString File::absoluteName(QString const &relativeName) {
    return Settings::instance()->pathPrefix() + relativeName;
}

QString File::percentViewed() const {
    if (!d->lastViewed.isValid() || d->runtimeSecs == 0)
        return "";
    return QString("%1").arg((uint) round(100 * d->lastPosition / d->runtimeSecs));
}

GREU_P_IMP(File, runtimeSecs, RuntimeSecs, uint)
GREU_P_IMP(File, lastPosition, LastPosition, double)
GREU_P_IMP(File, lastAudioTrack, LastAudioTrack, uint)
GREU_P_IMP(File, lastTextTrack, LastTextTrack, uint)
GREU_P_IMP(File, lastViewed, LastViewed, QDateTime)

Video *File::video() const {
    return d->video;
}

void File::setVideo(Video *v) {
    if (d->video == v)
        return;
    d->video = v;
}

movies::Movie *File::movie() const {
    return safeCast<Movie>(d->video);
}
movies::Episode *File::episode() const {
    return safeCast<Episode>(d->video);
}

void File::setFileInfo(QFileInfo const &fi) {
    d->fileInfo = fi;
}

} // namespace greu::movies
