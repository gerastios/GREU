#include "Modifiable.h"

using namespace std;

namespace {
mutex mxs, mxl;
bool tracking{true};
bool loud{true};
} // namespace

namespace greu::core {

struct Modifiable::Detail {
    bool modified = false;
    BoolSig modifiedChanged;
    mutex mx;
};

Modifiable::Modifiable()
    : d(GREU_MKDET()) {}

void Modifiable::setModified(bool v) {
    if (!tracking || d->modified == v)
        return;
    d->modified = v;
    if (::loud)
        d->modifiedChanged(v);
}

bool Modifiable::isModified() const {
    return d->modified;
}

bool Modifiable::save() {
    d->modified = false;
    return true;
}

BoolSig &Modifiable::modifiedChanged() const {
    return d->modifiedChanged;
}

bool Modifiable::isTracked() {
    lock_guard<mutex> guard{mxs};
    return tracking;
}

void Modifiable::enableTracking(bool v) {
    lock_guard<mutex> guard{mxs};
    tracking = v;
}

bool Modifiable::isLoud() {
    lock_guard<mutex> guard{mxl};
    return ::loud;
}

void Modifiable::loud() {
    lock_guard<mutex> guard{mxl};
    ::loud = true;
}

void Modifiable::quiet() {
    lock_guard<mutex> guard{mxl};
    ::loud = false;
}

} // namespace greu::core
