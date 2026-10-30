#include "PresentationItem.h"
#include "ListItem.h"
#include "MovieObjectItem.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct PresentationItem::Detail {
    Presentation const presentation;
    Detail(Presentation const &p)
        : presentation(p) {}
};

PresentationItem::PresentationItem(Presentation const &p)
    : d(GREU_MKDET(p)) {}

unique_ptr<PresentationItem> PresentationItem::create(Presentation const &p) {
    if (p.listType == ListType::None)
        return MovieObjectItem::create(p.groupTabs.at(0).uuids.at(0));
    return make_unique<ListItem>(p);
}

Presentation const &PresentationItem::presentation() const {
    return d->presentation;
}

} // namespace greu::gaion
