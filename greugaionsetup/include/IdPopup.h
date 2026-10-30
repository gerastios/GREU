#pragma once
#include "greuui/include/Dialog.h"

class QFileInfo;

namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) IdPopup : public ui::Dialog {
public:
    struct Id {
        int32_t id;
        uint seasonNumber, episodeNumber, variantSeasonNumber, variantEpisodeNumber;
        QString variantTitle;
    };

    IdPopup(QFileInfo const &, bool);
    Id id() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion::setup
