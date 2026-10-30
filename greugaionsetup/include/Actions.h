#pragma once
#include "greucore/include/DetailPtr.h"
#include "greucore/include/signalUtils.h"

// clang-format off
#define GREU_GAION_ACTION_SETUP_META(M) \
    M(about); \
    M(back); \
    M(exit); \
    M(objectSetImage); \
    M(createCollection); \
    M(deleteCollection); \
    M(addToCollection); \
    M(removeFromCollection); \
    M(forward); \
    M(playFile); \
    M(search); \
    M(settings); \
    M(updateAll); \
    M(updateFile); \
    M(updateObject);
// clang-format on

#define DEF(A) ui::Action &A() const

namespace greu::ui {
class Action;
}

namespace greu::gaion::setup {

class Dispatcher;

class GREU_PUBLIC(GAIONSETUP) Actions {
public:
    explicit Actions();

    GREU_GAION_ACTION_SETUP_META(DEF);

    core::StringSig &basePathChanged() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion::setup

#undef DEF
