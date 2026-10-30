#include "Timer.h"
#include <thread>

using namespace std::chrono;
using namespace std;

namespace greu::core {

struct Timer::Detail {
    EmptySig timeout;
    milliseconds timeoutSeconds;
    bool stopped = false, running = false;
    Detail(milliseconds to)
        : timeoutSeconds(to) {}
};

Timer::Timer(milliseconds to)
    : d(GREU_MKDET(to)) {}

void Timer::start() {
    d->stopped = false;
    thread t([this]() {
        d->running = true;
        auto start = steady_clock::now();
        while (true) {
            if (d->stopped) {
                d->running = false;
                return;
            }
            this_thread::sleep_for(std::chrono::milliseconds(500));
            if (d->stopped) {
                d->running = false;
                return;
            }
            auto curr = steady_clock::now();
            auto diff = duration_cast<milliseconds>(curr - start);
            if (diff > d->timeoutSeconds) {
                d->stopped = true;
                d->running = false;
                d->timeout();
                return;
            }
        }
    });
    t.detach();
}

void Timer::stop() {
    d->stopped = true;
}

EmptySig &Timer::timeout() const {
    return d->timeout;
}

} // namespace greu::core
