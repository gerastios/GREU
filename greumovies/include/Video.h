#pragma once
#include "CreditHolder.h"

namespace greu::movies {

struct GREU_PUBLIC(MOVIES) Video : public CreditHolder {
    using CreditHolder::CreditHolder;
    virtual ~Video() = default;
};

} // namespace greu::movies
