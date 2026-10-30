#pragma once
#include "MovieObjectItem.h"

namespace greu::movies {
class Episode;
}

namespace greu::gaion {

class GREU_PUBLIC(GAION) EpisodeItem : public MovieObjectItem {
public:
    EpisodeItem(Presentation const &);

    movies::Episode const *episode() const;

    virtual PresentationWidget *widget() const override;

    virtual FeatureModel *featureModel() const override;

    static std::unique_ptr<FeatureModel> featureModel(movies::Episode *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::gaion
