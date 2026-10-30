#pragma once

#include "greutag/include/FileHolder.h"
#include "greuui/include/Dialog.h"
#include <optional>

namespace greu::tag {
struct FileHolder;
class File;
} // namespace greu::tag

namespace greu::grylizo {

class FindTool : public ui::Dialog, public greu::tag::FileHolder {
public:
    FindTool(tag::FileHolder *);

    GREU_NO_MOVE(FindTool);
    GREU_NO_COPY(FindTool);

    tag::File *targetFile() const;

    virtual size_t size() const override;
    virtual greu::tag::File *at(size_t index) override;
    virtual std::optional<size_t> indexOf(greu::tag::File *) override;
    virtual void cforEach(ReaderFunction) const override;
    virtual void forEach(WriterFunction) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
