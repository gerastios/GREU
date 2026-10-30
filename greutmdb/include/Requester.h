#pragma once
#include "greunet/include/Requester.h"

namespace greu::tmdb {

struct GREU_PUBLIC(TMDB) Requester : public virtual net::Requester {
    virtual bool isApiKeyNeeded() const = 0;
};

} // namespace greu::tmdb
