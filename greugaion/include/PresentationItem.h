#pragma once
#include "NavigationController.h"

class QWidget;

namespace greu::core {
class Uuid;
}
namespace greu::gaion {

class PresentationWidget;

class GREU_PUBLIC(GAION) PresentationItem {
public:
    static std::unique_ptr<PresentationItem> create(Presentation const &);
    GREU_NO_COPY(PresentationItem);
    GREU_NO_MOVE(PresentationItem);
    virtual ~PresentationItem() = default;

    Presentation const &presentation() const;
    virtual PresentationWidget *widget() const = 0;

protected:
    PresentationItem(Presentation const &);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion
