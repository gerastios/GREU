#pragma once

#include "Media.h"
#include "greuui/include/TableView.h"

namespace greu::media {
class File;
}

namespace greu::geranos {

class WorkFileTable : public ui::TableView {
public:
    WorkFileTable();

    std::optional<Media> media() const;
    void clear();

protected:
    virtual void contextMenuEvent(QContextMenuEvent *) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::geranos
