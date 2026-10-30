#include "Saver.h"

using namespace std;

namespace greu::core {

struct Saver::Detail {
    bool all;
    exception_ptr exc;

    void work(promise<void> *const prms, Saver *that) {
        exc = nullptr;
        try {
            if (all)
                that->saveAll();
            else
                that->save();
        } catch (...) {
            exc = current_exception();
        }
        prms->set_value();
        that->finished()();
    }
};

Saver::Saver()
    : d(GREU_MKDET()) {}

void Saver::setSaveAll(bool v) {
    d->all = v;
}

bool Saver::isSaveAll() const {
    return d->all;
}

void Saver::work(std::promise<void> *const prms) {
    d->work(prms, this);
}

__exception_ptr::exception_ptr Saver::exception() {
    return d->exc;
}

} // namespace greu::core
