#pragma once
#include "MovieObjectWidget.h"

namespace greu::gaion {

class SeasonItem;

class GREU_PUBLIC(GAION) SeasonWidget : public MovieObjectWidget {
public:
    SeasonWidget(SeasonItem const *);
    virtual void addAtHeadInfoEnd(QVBoxLayout *) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual void addAfterHead(QVBoxLayout *) override;

};

} // namespace greu::gaion
