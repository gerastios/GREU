#include "Restorable.h"

namespace greu::core {

bool Restorable::save() {
    auto res = Modifiable::save();
    currentToOriginal();
    return res;
}

void Restorable::undoChanges() {
    originalToCurrent();
    setModified(false);
}

} // namespace greu::core
