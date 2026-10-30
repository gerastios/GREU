#pragma once
#include "PresentationWidget.h"

namespace greu::gaion {

struct FeatureModel;
class MovieObjectItem;

class GREU_PUBLIC(GAION) MovieObjectWidget : public PresentationWidget {
public:
    MovieObjectWidget(MovieObjectItem const *);
    virtual ~MovieObjectWidget() = default;

    virtual QWidget *widget() const override;

    MovieObjectItem const *item() const;

    virtual void addAtHeadInfoEnd(QVBoxLayout *) {}

    virtual std::any position() const override;
    virtual void setPosition(std::any const &) override;

protected:
    void init();

private:
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual void addAfterHead(QVBoxLayout *) {}

};
}
