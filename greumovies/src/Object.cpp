#include "Object.h"
#include <unordered_map>

using namespace std;
using namespace greu::core;

namespace {
std::unordered_map<greu::core::Uuid, std::unique_ptr<greu::movies::Object>> objects;
}

namespace greu::movies {

struct Object::Detail {
    Uuid const uuid;

    Detail(core::Uuid const &u)
        : uuid(u) {
        assert(!objects.contains(uuid));
    }
};

Object::Object(Uuid const &u)
    : d(GREU_MKDET(u)) {}

void Object::addObject(Uuid const &u, unique_ptr<Object> o) {
    objects[u] = std::move(o);
}

Object *Object::existingObject(Uuid const &u) {
    if (objects.contains(u))
        return objects.at(u).get();
    return nullptr;
}

bool Object::exists(Uuid const &u) {
    return objects.contains(u);
}

void Object::remove(Uuid const &u) {
    try {
        objects.erase(u);
    } catch (...) {
    }
}

Uuid const &Object::uuid() const {
    return d->uuid;
}

} // namespace greu::movies
