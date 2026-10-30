#pragma once
#include "MovieObjectWidget.h"

namespace greu::gaion {

class EpisodeItem;

class GREU_PUBLIC(GAION) EpisodeWidget : public MovieObjectWidget {
public:
    EpisodeWidget(EpisodeItem const *);
    virtual void addAtHeadInfoEnd(QVBoxLayout *) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual void addAfterHead(QVBoxLayout *) override;

};

} // namespace greu::gaion
