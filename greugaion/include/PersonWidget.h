#pragma once
#include "MovieObjectWidget.h"

namespace greu::gaion {

class PersonItem;

class GREU_PUBLIC(GAION) PersonWidget : public MovieObjectWidget {
public:
    PersonWidget(PersonItem const *);
    virtual void addAtHeadInfoEnd(QVBoxLayout *) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual void addAfterHead(QVBoxLayout *) override;

};

} // namespace greu::gaion
