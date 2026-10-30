#pragma once

#include "CompoundAttributeEditor.h"

namespace greu::grylizo {

class AdvancedEditor : public CompoundAttributeEditor {
public:
    explicit AdvancedEditor();

    virtual QWidget *setupUi() override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
