#include "LongRunnable.h"
#include <mutex>

using namespace std;

namespace greu::core {

struct LongRunnable::Detail {
    IntSig progressed;
    bool cancelled = false;
    VoidPromise promise;
    unique_ptr<std::thread> thread;
    std::mutex mtx;
};

LongRunnable::LongRunnable()
    : d(GREU_MKDET()) {}

IntSig &LongRunnable::progressed() const {
    return d->progressed;
}

LongRunnable::VoidFuture LongRunnable::future() {
    return d->promise.get_future();
}

void LongRunnable::start(LongRunnable::Worker *worker, bool synchroneous) {
    if (synchroneous)
        worker->work(&d->promise);
    else
        d->thread = make_unique<std::thread>(&Worker::work, worker, &d->promise);
}

void LongRunnable::cancel() {
    lock_guard<std::mutex> g(d->mtx);
    d->cancelled = true;
}

bool LongRunnable::isCancelled() const {
    return d->cancelled;
}

void LongRunnable::end() {
    lock_guard<std::mutex> g(d->mtx);
    d->thread->join();
}

} // namespace greu::core
