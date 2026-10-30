#pragma once
#include "MovieObjectWidget.h"

namespace greu::gaion {

class CollectionItem;

class GREU_PUBLIC(GAION) CollectionWidget : public MovieObjectWidget {
public:
    CollectionWidget(CollectionItem const *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual void addAfterHead(QVBoxLayout *) override;
};

} // namespace greu::gaion
