#pragma once

#include "Selector.h"

namespace greu::core {

class GREU_PUBLIC(CORE) StaticSelector: public Selector {
public:
    StaticSelector(InitializerList &list);
};

}
