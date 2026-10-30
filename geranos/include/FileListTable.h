#pragma once

#include "Media.h"
#include "greucore/include/DetailPtr.h"
#include "greucore/include/signalUtils.h"

class QString;
class QContextMenuEvent;

namespace greu::geranos {

class FileListTable {
public:
    explicit FileListTable(QString const &path);

    QWidget *setupUi();

    core::StringSig &externPlayRequested() const;
    using MediaSig = boost::signals2::signal<void(Media)>;
    MediaSig &trackInfoRequested() const;
    core::StringSig &backupRequested() const;
    core::StringSig &trackChapters() const;
    core::StringSig &editChapters() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::geranos
