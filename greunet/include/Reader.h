#pragma once
#include "greucore/include/greuGlobal.h"
#include <memory>

namespace greu::net {

struct Requester;
class Request;

class GREU_PUBLIC(NET) Reader {
public:
    virtual ~Reader() = default;
    void read(Requester *);

private:
    virtual std::unique_ptr<Request> request(Requester *) const = 0;
};

} // namespace greu::net
