#pragma once
#include "MovieObjectWidget.h"

namespace greu::gaion {

class MovieItem;

class GREU_PUBLIC(GAION) MovieWidget : public MovieObjectWidget {
public:
    MovieWidget(MovieItem const *);
    virtual void addAtHeadInfoEnd(QVBoxLayout *) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual void addAfterHead(QVBoxLayout *) override;

};

} // namespace greu::gaion
