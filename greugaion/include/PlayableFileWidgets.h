#pragma once
#include "greucore/include/DetailPtr.h"
#include "greucore/include/signalUtils.h"

class QVBoxLayout;

namespace greu::movies {
class File;
}

namespace greu::gaion {

class GREU_PUBLIC(GAION) PlayableFileWidgets {
public:
    PlayableFileWidgets(movies::File *, QVBoxLayout *);

    core::EmptySig &continueRequest() const;
    core::EmptySig &playRequest() const;
    core::EmptySig &playExtendedRequest() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

QString GREU_PUBLIC(GAION) runTime(int);

} // namespace greu::gaion
