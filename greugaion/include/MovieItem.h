#pragma once
#include "MovieObjectItem.h"

namespace greu::movies {
class Movie;
}

namespace greu::gaion {

class GREU_PUBLIC(GAION) MovieItem : public MovieObjectItem {
public:
    MovieItem(Presentation const &);

    movies::Movie const *movie() const;

    virtual PresentationWidget *widget() const override;

    virtual FeatureModel *featureModel() const override;

    static std::unique_ptr<FeatureModel> featureModel(movies::Movie *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::gaion
