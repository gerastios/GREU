#pragma once
#include "MovieObjectItem.h"

namespace greu::movies {
class Season;
}

namespace greu::gaion {

class GREU_PUBLIC(GAION) SeasonItem : public MovieObjectItem {
public:
    SeasonItem(Presentation const &);

    movies::Season const *season() const;

    virtual PresentationWidget *widget() const override;

    virtual FeatureModel *featureModel() const override;

    static std::unique_ptr<FeatureModel> featureModel(movies::Season const *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::gaion
