#pragma once

#include "DetailPtr.h"
#include "signalUtils.h"

namespace greu::core {

class GREU_PUBLIC(CORE) Modifiable {

public:
    Modifiable();

    virtual void setModified(bool);
    virtual bool isModified() const;
    virtual bool save();

    virtual BoolSig &modifiedChanged() const;

    static bool isTracked();
    static void enableTracking(bool = true);

    static bool isLoud();
    static void loud();
    static void quiet();

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

}

