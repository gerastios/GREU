#pragma once

#include "greucore/include/DetailPtr.h"

class QWidget;

namespace greu::tag {
class FileHolder;
class File;
} // namespace greu::tag

namespace greu::grylizo {

class DuplicateTool {
public:
    DuplicateTool(tag::FileHolder *files);

    GREU_NO_MOVE(DuplicateTool);
    GREU_NO_COPY(DuplicateTool);

    bool execute();
    tag::File *targetFile() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
