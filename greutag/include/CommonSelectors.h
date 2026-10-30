#pragma once
#include "greucore/include/greuGlobal.h"

namespace greu::core {
class PlainSelector;
}

namespace greu::tag {

class GREU_PUBLIC(TAG) CommonSelectors {
public:
    CommonSelectors() = delete;

public:
    static core::PlainSelector *genres();
    static core::PlainSelector *moods();
    static core::PlainSelector *instruments();
    static core::PlainSelector *peopleFunctions();
};

} // namespace greu::tag
