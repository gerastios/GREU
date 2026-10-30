#include "Reader.h"
#include <QCoreApplication>
#include "Request.h"
#include "Requester.h"
#include "greucore/include/LongRunnable.h"

using namespace std;
using namespace greu::net;
using namespace greu::core;

namespace {

struct IntReader : public LongRunnable, public LongRunnable::Worker {
    Request *request;
    __exception_ptr::exception_ptr except;

    IntReader(Request *r)
        : LongRunnable()
        , request(r) {}

    virtual void work(VoidPromise *promise) override {
        try {
            request->request();
        } catch (...) {
            except = current_exception();
        }
        promise->set_value();
        finished()();
    }

    virtual __exception_ptr::exception_ptr exception() override { return except; }
};

} // namespace

namespace greu::net {

void Reader::read(Requester *requester) {
    auto req = request(requester);
    IntReader reader{req.get()};
    auto finished = false;
    reader.finished().connect([&finished] { finished = true; });
    auto future = reader.future();
    reader.start(&reader, false);
    while (!finished)
        this_thread::sleep_for(chrono::milliseconds(5));
    future.get();
    reader.end();
    auto e = reader.exception();
    if (e)
        rethrow_exception(e);
}

} // namespace greu::net
