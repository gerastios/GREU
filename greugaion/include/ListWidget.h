#pragma once
#include <QWidget>
#include "ListItem.h"
#include "PresentationWidget.h"

namespace greu::gaion {

class GREU_PUBLIC(GAION) ListWidget : public PresentationWidget {
public:
    ListWidget(ListItem const *);

    virtual QWidget *widget() const override;
    ListItem const *listItem() const;

    virtual std::any position() const override;
    virtual void setPosition(std::any const &) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};
} // namespace greu::gaion
