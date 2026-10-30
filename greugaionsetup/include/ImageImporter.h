#pragma once
#include "greucore/include/greuGlobal.h"

namespace greu::ui {
class ImageContainer;
}
namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) ImageImporter {
public:
    ImageImporter() = delete;

    static ui::ImageContainer import();
};

} // namespace greu::gaion
