#pragma once
#include "greucore/include/DetailPtr.h"
#include "greucore/include/Modifiable.h"
#include <vector>

class QFileInfo;
class QFile;

namespace greu::core {
class Log;
}

namespace greu::tag {

class AttributeCollection;
class Tag;
class Album;
class Directory;

class GREU_PUBLIC(TAG) File: public core::Modifiable {
public:
    explicit File(QFileInfo const &);
    virtual ~File() = default;

    virtual Tag * tag() const = 0;
    const QFileInfo & info() const;
    void updateInfo();

    void analyze();

    void setAlbum(Album *);
    Album * album() const;
    void setDirectory(Directory *);
    Directory * directory() const;

    virtual bool save() override;

    core::Log &log() const;

private:
    virtual void analyzeFile(QFile *) = 0;
    virtual bool saveContent() = 0;

    struct Detail;
    core::DetailPtr<Detail> d;

};

void GREU_PUBLIC(TAG) sort(std::vector<File *>::iterator filesBegin, std::vector<File *>::iterator filesEnd, uint typeId, Qt::SortOrder order);

}
