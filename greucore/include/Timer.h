#pragma once
#include "DetailPtr.h"
#include "signalUtils.h"

namespace greu::core {

class GREU_PUBLIC(CORE) Timer {
public:
    Timer(std::chrono::milliseconds);
    void start();
    void stop();

    EmptySig &timeout() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::core
