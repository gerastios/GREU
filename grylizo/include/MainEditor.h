#pragma once

#include "CompoundAttributeEditor.h"

namespace greu::tag {
class File;
class AlbumChanger;
} // namespace greu::tag

namespace greu::grylizo {

class MainEditor : public CompoundAttributeEditor {
public:
    explicit MainEditor();

    virtual QWidget *setupUi() override;
    virtual void update() override;

    void albumChange(tag::AlbumChanger *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
