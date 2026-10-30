#pragma once

#include "greucore/include/DetailPtr.h"

namespace greu::grylizo {

class Actions {
public:
    Actions();

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

}

