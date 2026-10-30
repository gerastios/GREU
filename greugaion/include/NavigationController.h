#pragma once
#include "Presentation.h"
#include "greucore/include/DetailPtr.h"
#include "greucore/include/signalUtils.h"

namespace greu::gaion {

class PresentationItem;

class GREU_PUBLIC(GAION) NavigationController {
public:
    static NavigationController *instance();

    Presentation const *current() const;
    void setCurrent(Presentation const &);
    PresentationItem *currentItem() const;

    bool forwardPossible() const;
    bool backwardPossible() const;
    void forward();
    void backward();

    core::EmptySig &currentSet() const;

private:
    NavigationController();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion

#define GREU_NAVC greu::gaion::NavigationController::instance()
