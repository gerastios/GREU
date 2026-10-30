#pragma once
#include "greunet/include/Request.h"

namespace greu::tmdb {

struct Requester;

class GREU_PUBLIC(TMDB) Request : public net::Request {
public:
    Request(tmdb::Requester *);

private:
    virtual void completeQuery(QUrlQuery *) override;
};

} // namespace greu::tmdb
