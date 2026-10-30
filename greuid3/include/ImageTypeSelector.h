#pragma once
#include "greucore/include/Selector.h"
#include "greucore/include/greuGlobal.h"

namespace greu::id3 {

class GREU_PUBLIC(ID3) ImageTypeSelector : public core::Selector {
public:
    static uint const fileIcon;
    static uint const fileOtherIcon;
    static uint const frontCover;

    static ImageTypeSelector *instance();

private:
    ImageTypeSelector();
};

} // namespace greu::id3
