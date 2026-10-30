#include "ModifiableCollection.h"
#include <unordered_map>

using namespace std;

namespace greu::core {

struct ModifiableCollection::Detail {
    unordered_map<Modifiable *, ScopedConnection> modifiables;
};

ModifiableCollection::ModifiableCollection()
    : d(GREU_MKDET()) {}

void ModifiableCollection::add(Modifiable *mod) {
    if (!d->modifiables.contains(mod)) {
        d->modifiables[mod] = mod->modifiedChanged().connect([this](bool c) {
            if (c || isModified())
                setModified(true);
            else
                setModified(false);
        });
    }
}

void ModifiableCollection::remove(Modifiable *mod) {
    try {
        auto &m = d->modifiables.at(mod);
        m.disconnect();
        d->modifiables.erase(mod);
    } catch (out_of_range &) {
        return;
    }
}

void ModifiableCollection::clear() {
    d->modifiables.clear();
}

void ModifiableCollection::setModified(bool v) {
    if (!v)
        for (auto &m : d->modifiables)
            m.first->setModified(false);
    Modifiable::setModified(v);
}

bool ModifiableCollection::isModified() const {
    for (auto &m : d->modifiables) {
        if (m.first->isModified())
            return true;
    }
    return false;
}

bool ModifiableCollection::save() {
    bool res = true;
    for (auto &m : d->modifiables) {
        if (!m.first->save())
            res = false;
    }
    return res;
}
} // namespace greu::core
