#include "Request.h"
#include <QByteArray>
#include <QEventLoop>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QString>
#include <QUrlQuery>
#include "Exception.h"
#include "greuui/include/connect.h"

using namespace std;

namespace greu::net {

using namespace core;

struct Request::Detail {
    Requester *requester;

    Detail(Requester *requester)
        : requester(requester) {}

    void request(Request *that) {
        QNetworkAccessManager manager;
        auto query = requester->query();
        that->completeQuery(&query);
        auto url = requester->url();
        if (!query.isEmpty())
            url.setQuery(query);
        auto req = QNetworkRequest{url};
        QEventLoop loop;
        req.setAttribute(QNetworkRequest::RedirectPolicyAttribute,
                         QNetworkRequest::NoLessSafeRedirectPolicy);
        auto as = requester->acceptString();
        if (!as.isEmpty())
            req.setRawHeader("accept", as.toLatin1());
        auto reply = manager.get(req);
        assert(reply);
        bool errorOccured = false;
        QString errorResponse;
        ui::connectFinished(reply, [&loop, &errorOccured] {
            if (!errorOccured) {
                loop.quit();
            }
        });
        ui::connectOnError(reply, [&errorOccured, &loop](QNetworkReply::NetworkError) {
            errorOccured = true;
            loop.quit();
        });
        loop.exec();
        if (errorOccured) {
            errorResponse = reply->readAll();
            throw Exception{reply->error(), reply->errorString(), req.url(), errorResponse};
        } else {
            requester->finished(reply);
        }
    }
};

Request::Request(Requester *requester)
    : d(GREU_MKDET(requester)) {}

void Request::request() {
    d->request(this);
}

Requester *Request::requester() const {
    return d->requester;
}

} // namespace greu::net
