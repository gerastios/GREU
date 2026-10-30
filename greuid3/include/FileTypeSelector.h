#pragma once
#include "greucore/include/PlainSelector.h"

namespace greu::id3 {

class GREU_PUBLIC(ID3) FileTypeSelector : public core::PlainSelector {
public:
    static FileTypeSelector *instance();

private:
    FileTypeSelector();
};

} // namespace greu::id3
