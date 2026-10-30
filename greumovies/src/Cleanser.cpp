#include "Cleanser.h"
#include "Cleanable.h"
#include "greucore/include/vectorUtils.h"
#include <vector>

using namespace std;

namespace greu::movies {

using namespace core;

struct Cleanser::Detail {
    vector<Cleanable *> cleanables;
};

Cleanser *Cleanser::instance() {
    static Cleanser i;
    return &i;
}

void Cleanser::add(Cleanable *c) {
    if(!contains(d->cleanables, c))
        d->cleanables.push_back(c);
}

void Cleanser::clean() {
    auto finished = false;
    while (!finished) {
        finished = true;
        for (auto &c : d->cleanables)
            finished &= c->clean() == 0;
    }
}

Cleanser::Cleanser()
    : d(GREU_MKDET()) {}

} // namespace greu::movies
