#pragma once
#include "greunet/include/Reader.h"

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) Reader : public net::Reader {
private:
    virtual std::unique_ptr<net::Request> request(net::Requester *) const override;
};

} // namespace greu::tmdb
