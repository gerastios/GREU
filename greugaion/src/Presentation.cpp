#include "Presentation.h"

using namespace std;

namespace greu::gaion {

using namespace core;

Presentation::Presentation(Uuid const &u)
    : groupTabs({GroupTab{u}}) {}

Presentation::Presentation(QString const &t, ListType lt, GroupTabs const &tb)
    : title(t)
    , listType(lt)
    , groupTabs(tb) {}

size_t Presentation::size() const {
    size_t res = 0;
    for (auto &t : groupTabs)
        res += t.uuids.size();
    return res;
}

bool Presentation::operator==(Presentation const &other) const {
    return title == other.title && listType == other.listType && groupTabs == other.groupTabs;
}

Uuid Presentation::singleUuid() const {
    assert(isSingle());
    return groupTabs.at(0).uuids.at(0);
}

Presentation::Presentation(Uuids const &u)
    : groupTabs({{u}}) {}

Presentation::Presentation(QString const &t, ListType lt, Uuids const &u)
    : title(t)
    , listType(lt)
    , groupTabs({u}) {}

Presentation::GroupTab::GroupTab(Uuid const &u)
    : uuids({u}) {}

Presentation::GroupTab::GroupTab(Uuids const &u)
    : uuids(u) {}

Presentation::GroupTab::GroupTab(QString const &t, Uuids const &u)
    : title(t)
    , uuids(u) {}

bool Presentation::isEmpty() const {
    return groupTabs.size() == 0;
}

bool Presentation::isSingle() const {
    return groupTabs.size() == 1 && groupTabs.at(0).uuids.size() == 1;
}

bool Presentation::hasUuid() const {
    if (groupTabs.empty())
        return false;
    for (auto const &t : groupTabs)
        if (!t.uuids.empty())
            return true;
    return false;
}

} // namespace greu::gaion
