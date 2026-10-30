#pragma once
#include <QNetworkReply>
#include <QJsonParseError>
#include "greucore/include/greuGlobal.h"

namespace greu::net {

struct GREU_PUBLIC(NET) Requester {
    virtual ~Requester() = default;
    virtual QUrl url() const = 0;
    virtual QUrlQuery query() const = 0;
    virtual QString acceptString() const = 0;
    virtual void finished(QNetworkReply *) = 0;
};

}

