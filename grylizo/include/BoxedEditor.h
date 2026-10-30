#pragma once

#include <QString>
#include "CompoundAttributeEditor.h"

namespace greu::grylizo {

class BoxedEditor : public CompoundAttributeEditor {
public:
    BoxedEditor(QString const & = "");
    virtual QWidget *setupUi() override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
