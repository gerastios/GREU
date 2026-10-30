#pragma once
#include "greuui/include/ComboInput.h"

namespace greu::movies {
class Collection;
}

namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) CollectionDialog : public ui::ComboInput {
public:
    CollectionDialog();
    movies::Collection *collection() const;
};

} // namespace greu::gaion
