#pragma once
#include "greuui/include/Dialog.h"

namespace greu::movies {
struct SearchOptions;
}

namespace greu::gaion::app {

class GREU_PUBLIC(GAIONAPP) SearchDialog : public ui::Dialog {
public:
    SearchDialog();

    movies::SearchOptions options() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion
