#pragma once
#include "MovieObjectWidget.h"

namespace greu::gaion {

class MovieCollectionItem;

class GREU_PUBLIC(GAION) MovieCollectionWidget : public MovieObjectWidget {
public:
    MovieCollectionWidget(MovieCollectionItem const *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual void addAfterHead(QVBoxLayout *) override;
};

} // namespace greu::gaion
