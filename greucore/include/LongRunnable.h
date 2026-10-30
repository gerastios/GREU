#pragma once

#include "Runnable.h"
#include <boost/signals2.hpp>
#include <future>
#include <exception>

namespace greu::core {

class GREU_PUBLIC(CORE) LongRunnable: public Runnable {

public:
    using VoidFuture = std::future<void>;
    using VoidPromise = std::promise<void>;

    struct Worker {
        virtual void work(VoidPromise * promise) = 0;
        virtual std::exception_ptr exception() = 0;
    };

    LongRunnable();

    IntSig &progressed() const;

    VoidFuture future();
    void start(Worker *worker, bool synchroneous);
    void cancel();
    bool isCancelled() const;
    void end();

private:
    struct Detail;
    DetailPtr<Detail> d;

};

}
