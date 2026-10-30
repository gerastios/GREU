#pragma once
#include "Requester.h"
#include "greucore/include/DetailPtr.h"

namespace greu::net {

class Reader;

class GREU_PUBLIC(NET) JsonRequester: public virtual Requester {
public:
    JsonRequester();
    virtual QString acceptString() const override;
    virtual void finished(QNetworkReply *) override;
    QJsonDocument readJson(Reader *);

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

}

