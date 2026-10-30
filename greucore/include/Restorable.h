#pragma once

#include "Modifiable.h"

namespace greu::core {

struct GREU_PUBLIC(CORE) Restorable : public Modifiable {
    virtual void originalToCurrent() = 0;
    virtual void currentToOriginal() = 0;
    virtual bool save() override;
    virtual void undoChanges();
};

} // namespace greu::core
