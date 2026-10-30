#pragma once
#include "greumovies/include/OwnMovie.h"
#include "greuui/include/Dialog.h"

namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) OwnGenresEditor : public ui::Dialog {
public:
    OwnGenresEditor(movies::OwnMovie const &);

    movies::OwnMovie ownMovie() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion::setup
