#pragma once
#include "greucore/include/DetailPtr.h"

namespace greu::movies {

struct Cleanable;

class GREU_PUBLIC(MOVIES) Cleanser {
public:
    static Cleanser *instance();
    void add(Cleanable *);
    void clean();

private:
    Cleanser();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies
