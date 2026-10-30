#include "Reader.h"
#include "Request.h"
#include "Requester.h"
#include "greucore/include/utils.h"

using namespace std;

namespace greu::tmdb {

using namespace core;

unique_ptr<net::Request> Reader::request(net::Requester *r) const {
    auto res = unique_ptr<tmdb::Request>(new Request(safeCast<tmdb::Requester>(r)));
    return res;
}

} // namespace greu::tmdb
