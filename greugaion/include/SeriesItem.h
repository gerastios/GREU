#pragma once
#include "MovieObjectItem.h"

namespace greu::movies {
class Series;
}

namespace greu::gaion {

class GREU_PUBLIC(GAION) SeriesItem : public MovieObjectItem {
public:
    SeriesItem(Presentation const &);

    movies::Series const *series() const;

    virtual PresentationWidget *widget() const override;

    virtual FeatureModel *featureModel() const override;

    static std::unique_ptr<FeatureModel> featureModel(movies::Series *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::gaion
