#pragma once
#include "MovieObjectItem.h"

namespace greu::movies {
class Collection;
}

namespace greu::gaion {

class GREU_PUBLIC(GAION) CollectionItem : public MovieObjectItem {
public:
    CollectionItem(Presentation const &);

    movies::Collection const *collection() const;

    virtual PresentationWidget *widget() const override;

    virtual FeatureModel *featureModel() const override;

    static std::unique_ptr<FeatureModel> featureModel(movies::Collection *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::gaion
