#pragma once

#include "greuui/include/Dialog.h"
#include <vector>

namespace greu::tag {
class File;
}

namespace greu::grylizo {

class NormalizeTool : public ui::Dialog {
public:
    NormalizeTool(std::vector<tag::File *> const &);

    GREU_NO_MOVE(NormalizeTool);
    GREU_NO_COPY(NormalizeTool);

protected:
    virtual void closeEvent(QCloseEvent *event) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
