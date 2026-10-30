#pragma once
#include "Cleanable.h"
#include "greucore/include/Uuid.h"

namespace greu::movies {

class Movie;
class Episode;
class File;
struct Video;

class GREU_PUBLIC(MOVIES) FilesTable : public Cleanable {
public:
    static FilesTable *instance();
    virtual ~FilesTable() = default;

    std::pair<bool, core::Uuid> isAssigned(QString const &);
    File *readForFileSystem(QString const &absoluteFileName);

    File *read(core::Uuid const &);
    void read(File *);
    void save(File const *);
    void saveLastViewed(File const *);

    std::optional<core::Uuid> fileOfVideo(Video const *) const;

    virtual uint64_t clean() override;

private:
    FilesTable();
    void read(File *, Record const &);
};

} // namespace greu::movies
