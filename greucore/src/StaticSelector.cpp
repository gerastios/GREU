#include "StaticSelector.h"

namespace greu::core {

StaticSelector::StaticSelector(InitializerList &list) {
    for(auto v: list)
        insertInit(v);
    allowDeletion(false);
    allowInsertion(false);
    allowUserValues(false);
}

}
