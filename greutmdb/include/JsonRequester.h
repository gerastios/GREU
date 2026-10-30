#pragma once
#include "Requester.h"
#include "greunet/include/JsonRequester.h"

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) JsonRequester : public virtual net::JsonRequester, public virtual tmdb::Requester {
    inline virtual bool isApiKeyNeeded() const override { return true; }
};

} // namespace greu::tmdb
