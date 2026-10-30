#pragma once
#include "MovieObjectItem.h"

namespace greu::movies {
class Person;
}

namespace greu::gaion {

class GREU_PUBLIC(GAION) PersonItem : public MovieObjectItem {
public:
    PersonItem(Presentation const &);

    movies::Person const *person() const;

    virtual PresentationWidget *widget() const override;

    virtual FeatureModel *featureModel() const override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::gaion
