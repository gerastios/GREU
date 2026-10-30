#include "Runnable.h"

namespace greu::core {

struct Runnable::Detail {
    EmptySig finished;
};

Runnable::Runnable()
    : d(GREU_MKDET()) {}

EmptySig &Runnable::finished() const {
    return d->finished;
}

} // namespace greu::core
