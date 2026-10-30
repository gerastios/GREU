#include "LinkController.h"
#include "Presentation.h"
#include "greucore/include/Uuid.h"
#include <unordered_map>

using namespace std;

namespace greu::gaion {

using namespace core;

struct LinkController::Detail {
    unordered_map<QString, Presentation const> links;
};

LinkController::LinkController()
    : d(GREU_MKDET()) {}

QString LinkController::buildLink(Presentation const p) {
    QString l = Uuid();
    d->links.insert({l, std::move(p)});
    return l;
}

Presentation LinkController::presentation(QString const &l) const {
    try {
        return d->links.at(l);
    } catch (...) {
        assert(false);
    }
}

} // namespace greu::gaion
