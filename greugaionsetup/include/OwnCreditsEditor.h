#pragma once
#include "greumovies/include/OwnMovie.h"
#include "greuui/include/Dialog.h"

namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) OwnCreditsEditor : public ui::Dialog {
public:
    enum Type {
        cast,
        crew,
    };

    OwnCreditsEditor(movies::OwnMovie const &, Type);

    movies::OwnMovie ownMovie() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

}
