#include "ObjectState.h"

namespace greu::movies {

using namespace core;

struct ObjectState::Detail {
    bool isNew = false, isDeleted = false;
};

ObjectState::ObjectState()
    : d(GREU_MKDET()) {}

void ObjectState::setNew(bool v) {
    d->isNew = v;
}
bool ObjectState::isNew() const {
    return d->isNew;
}
void ObjectState::setDeleted(bool v) {
    d->isDeleted = v;
}
bool ObjectState::isDeleted() const {
    return d->isDeleted;
}

} // namespace greu::movies
