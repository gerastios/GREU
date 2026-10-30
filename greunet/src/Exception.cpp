#include "Exception.h"

namespace greu::net {

using namespace core;

struct Exception::Detail {
    QNetworkReply::NetworkError const code;

    Detail(QNetworkReply::NetworkError const code)
        : code(code) {}
};

Exception::Exception(QNetworkReply::NetworkError const code,
                     QString const &text,
                     QUrl const &url,
                     QString const &response)
    : core::Exception(text + "\n" + url.toString() + "\n" + response)
    , d(GREU_MKDET(code)) {}

QNetworkReply::NetworkError Exception::code() const {
    return d->code;
}

} // namespace greu::net
