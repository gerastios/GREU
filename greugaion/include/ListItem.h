#pragma once
#include "PresentationItem.h"

namespace greu::gaion {

class MovieObjectItem;

class GREU_PUBLIC(GAION) ListItem : public PresentationItem {
public:
    ListItem(Presentation const &);

    virtual PresentationWidget *widget() const override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::gaion
