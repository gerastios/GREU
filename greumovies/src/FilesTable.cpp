#include "FilesTable.h"
#include <QFileInfo>
#include <QVariant>
#include "Episode.h"
#include "File.h"
#include "Movie.h"
#include "Strings.h"
#include "datatypes.h"
#include "greusql/include/Query.h"

using namespace std;
using namespace greu::core;
using namespace greu::sql;
using namespace greu::movies;

namespace {
// clang-format off
static size_t constexpr SLINE = __LINE__ + 1,
    ID = __LINE__ - SLINE,
    FILE_NAME = __LINE__ - SLINE,
    MEDIUM_TYPE = __LINE__ - SLINE,
    MEDIUM_ID = __LINE__ - SLINE,
    RUNTIME = __LINE__ - SLINE,
    LAST_POSITION = __LINE__ - SLINE,
    LAST_AUDIO = __LINE__ - SLINE,
    LAST_TEXT = __LINE__ - SLINE,
    LAST_VIEWED = __LINE__ - SLINE;
// clang-format on

} // namespace

namespace greu::movies {

FilesTable::FilesTable()
    : Cleanable("files",
                {
                    {Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                    {"file_name", GREU_MVDB_TYPE_FILENAME},
                    {"medium_type", GREU_MVDB_TYPE_TYPE},
                    {"medium_id", GREU_MVDB_TYPE_ID},
                    {"runtime", GREU_MVDB_TYPE_NUMBER},
                    {"last_position", DataType::DOUBLE, 53},
                    {"last_audio", GREU_MVDB_TYPE_NUMBER},
                    {"last_text", GREU_MVDB_TYPE_NUMBER},
                    {"last_viewed", GREU_MVDB_TYPE_DATETIME},
                },
                {{"medium_index", {"medium_type", "medium_id"}, true},
                 {"medium_id_index", {"medium_id"}},
                 {"file_index", {"file_name"}, true}}) {
    create();
}

void FilesTable::read(File *file, Record const &v) {
    auto type = v[MEDIUM_TYPE].toInt();
    if (type == (int) File::Type::episode) {
        file->setVideo(Episode::episode(v[MEDIUM_ID]));
    } else {
        file->setVideo(Movie::movie(v[MEDIUM_ID]));
    }
    file->setRuntimeSecs(v[RUNTIME].toUInt());
    file->setLastPosition(v[LAST_POSITION].toDouble());
    file->setLastAudioTrack(v[LAST_AUDIO].toUInt());
    file->setLastTextTrack(v[LAST_TEXT].toUInt());
    file->setLastViewed(v[LAST_VIEWED].toDateTime());
    file->setFileInfo(QFileInfo{File::absoluteName(v[FILE_NAME].toString())});
}

FilesTable *FilesTable::instance() {
    static FilesTable i;
    return &i;
}

pair<bool, core::Uuid> FilesTable::isAssigned(QString const &fileName) {
    auto r = selectEqual({fileName}, {&fields()[FILE_NAME]});
    if (r.size() == 0)
        return {false, Uuid::emptyUuid()};
    else
        return {true, r[0][ID]};
}

File *FilesTable::readForFileSystem(QString const &absoluteFileName) {
    auto [ass, uuid] = isAssigned(File::relativeName(absoluteFileName));
    File *res;
    if (ass) {
        res = File::file(uuid);
    } else {
        res = Object::object<File>(Uuid{});
    }
    res->setFileInfo(QFileInfo(absoluteFileName));
    return res;
}

File *FilesTable::read(Uuid const &u) {
    if (Object::exists(u))
        return Object::object<File>(u);
    return Object::object<File>(u);
}

void FilesTable::read(File *file) {
    auto o = selectSingle({file->uuid()});
    if (!o)
        return;
    auto v = o.value();
    read(file, v);
}

void FilesTable::save(File const *file) {
    if (file->isDeleted() || file->mediumType() == File::Type::none) {
        deleteSingle({file->uuid()});
        return;
    }
    auto v = file->video();
    insertOrUpdate({file->uuid(),
                    file->relativeName(),
                    (int) file->mediumType(),
                    v ? v->uuid() : Uuid::emptyUuid(),
                    file->runtimeSecs(),
                    file->lastPosition(),
                    file->lastAudioTrack(),
                    file->lastTextTrack(),
                    file->lastViewed()});
}

void FilesTable::saveLastViewed(File const *file) {
    update({file->uuid()},
           {file->lastPosition(), file->lastAudioTrack(), file->lastTextTrack(), file->lastViewed()},
           {&fields()[LAST_POSITION],
            &fields()[LAST_AUDIO],
            &fields()[LAST_TEXT],
            &fields()[LAST_VIEWED]});
}

optional<Uuid> FilesTable::fileOfVideo(Video const *m) const {
    auto recs = selectEqual({m->uuid()}, {&fields()[MEDIUM_ID]});
    if (recs.size() == 0)
        return nullopt;
    return recs[0][ID];
}

uint64_t FilesTable::clean() {
    auto q = Query::prepareQuery("select distinct id from files where medium_type = 1 and not "
                                 "exists (select * from movies where "
                                 "id = files.medium_id);");
    auto res = deleteRecords(fields().at(ID), selectToDelete(q));
    q = Query::prepareQuery("select distinct id from files where medium_type = 2 and not exists "
                            "(select * from episodes where "
                            "id = files.medium_id);");
    res += deleteRecords(fields().at(ID), selectToDelete(q));
    return res;
}

} // namespace greu::movies
