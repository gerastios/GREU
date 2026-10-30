#pragma once
#include "greucore/include/PlainSelector.h"

namespace greu::id3 {

class GREU_PUBLIC(ID3) MediaTypeSelector : public core::PlainSelector {
public:
    static MediaTypeSelector *instance();

private:
    MediaTypeSelector();
};

} // namespace greu::id3
