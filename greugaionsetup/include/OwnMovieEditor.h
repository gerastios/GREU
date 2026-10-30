#pragma once
#include <QDate>
#include "greumovies/include/OwnMovie.h"
#include "greuui/include/Dialog.h"

namespace greu::ui {
class ImageContainer;
}

namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) OwnMovieEditor : public ui::Dialog {
public:
    OwnMovieEditor();

    void setOwnMovie(movies::OwnMovie const &);
    movies::OwnMovie const &ownMovie() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion::setup
