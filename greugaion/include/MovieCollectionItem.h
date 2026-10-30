#pragma once
#include "MovieObjectItem.h"

namespace greu::movies {
class MovieCollection;
}

namespace greu::gaion {

class GREU_PUBLIC(GAION) MovieCollectionItem : public MovieObjectItem {
public:
    MovieCollectionItem(Presentation const &);

    movies::MovieCollection const *movieCollection() const;

    virtual PresentationWidget *widget() const override;

    virtual FeatureModel *featureModel() const override;

    static std::unique_ptr<FeatureModel> featureModel(movies::MovieCollection *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::gaion
