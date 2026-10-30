#pragma once
#include "MovieObjectWidget.h"

namespace greu::gaion {

class SeriesItem;

class GREU_PUBLIC(GAION) SeriesWidget : public MovieObjectWidget {
public:
    SeriesWidget(SeriesItem const *);
    virtual void addAtHeadInfoEnd(QVBoxLayout *) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual void addAfterHead(QVBoxLayout *) override;

};

} // namespace greu::gaion
