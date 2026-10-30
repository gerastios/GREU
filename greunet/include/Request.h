#pragma once
#include "greucore/include/DetailPtr.h"
#include "Requester.h"

namespace greu::net {

class GREU_PUBLIC(NET) Request {
public:
    Request(Requester *);
    virtual ~Request() = default;
    GREU_NO_COPY(Request);
    GREU_NO_MOVE(Request);

    void request();

    Requester * requester() const;

private:
    virtual void completeQuery(QUrlQuery *) {}
    struct Detail;
    core::DetailPtr<Detail> d;

};

}

