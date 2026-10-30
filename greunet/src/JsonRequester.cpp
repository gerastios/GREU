#include "JsonRequester.h"
#include "Reader.h"
#include "JsonException.h"
#include <QJsonDocument>

using namespace std;

namespace {
QString const jsonAccept{"application/json"};
}

namespace greu::net {

using namespace core;

struct JsonRequester::Detail {
    QJsonDocument json;
};

JsonRequester::JsonRequester(): d(GREU_MKDET()) {}

QString JsonRequester::acceptString() const { return jsonAccept; }

void JsonRequester::finished(QNetworkReply * reply) {
    QJsonParseError error;
    d->json = QJsonDocument::fromJson(reply->readAll(), &error);
    if(error.error != error.NoError) throw JsonException{error};
}

QJsonDocument JsonRequester::readJson(Reader * r) {
    r->read(this);
    return d->json;
}

}
