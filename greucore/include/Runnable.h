#pragma once

#include "DetailPtr.h"
#include "signalUtils.h"

namespace greu::core {

class GREU_PUBLIC(CORE) Runnable {
public:
    Runnable();
    GREU_NO_COPY(Runnable);

    EmptySig &finished() const;

private:
    struct Detail;
    DetailPtr<Detail> d;
};

} // namespace greu::core
