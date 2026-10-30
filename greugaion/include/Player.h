#pragma once
#include "greucore/include/DetailPtr.h"
#include "greucore/include/signalUtils.h"

namespace greu::movies {
class File;
}

namespace greu::gaion {

class GREU_PUBLIC(GAION) Player {
public:
    Player(movies::File *);

    void play();
    void playExtended();
    void playContinue();

    core::EmptySig &finished() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion
