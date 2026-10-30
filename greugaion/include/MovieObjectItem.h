#pragma once
#include "PresentationItem.h"

namespace greu::movies {
class MovieObject;
}

namespace greu::gaion {

struct FeatureModel;

class GREU_PUBLIC(GAION) MovieObjectItem : public PresentationItem {
public:
    static std::unique_ptr<MovieObjectItem> create(core::Uuid const &);
    static movies::MovieObject *object(core::Uuid const &);
    static movies::MovieObject *object(Presentation const &);
    movies::MovieObject const *object() const;

    virtual FeatureModel *featureModel() const = 0;

protected:
    MovieObjectItem(Presentation const &);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion
