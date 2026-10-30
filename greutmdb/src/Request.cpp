#include "Request.h"
#include <QUrlQuery>
#include "Requester.h"
#include "TheMovieDatabase.h"
#include "greucore/include/utils.h"

namespace greu::tmdb {

using namespace core;

Request::Request(Requester *r)
    : net::Request(r) {}

void Request::completeQuery(QUrlQuery *q) {
    if (safeCast<Requester>(requester())->isApiKeyNeeded())
        q->addQueryItem("api_key", GREU_TMDB->apiKey());
}

} // namespace greu::tmdb
