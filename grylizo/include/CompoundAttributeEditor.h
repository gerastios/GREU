#pragma once

#include "AbstractAttributeEditor.h"
#include "greucore/include/DetailPtr.h"
#include <vector>

namespace greu::grylizo {

class CompoundAttributeEditor : public AbstractAttributeEditor {
public:
    explicit CompoundAttributeEditor();

    AbstractAttributeEditor *addEditor(std::unique_ptr<AbstractAttributeEditor> &&);

    virtual void update() override;
    virtual void commit() override;

    std::vector<std::unique_ptr<AbstractAttributeEditor>>::iterator begin();
    std::vector<std::unique_ptr<AbstractAttributeEditor>>::iterator end();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
