#include "UrlTypeSelector.h"

using namespace std;

namespace greu::id3 {

namespace {

uint const userValueType = 0;

unordered_map<QString, UrlTypeSelector::TypeDescription> const types
    = {{"WXXX", {userValueType, QObject::tr("User defined"), true, true}},
       {"WCOM", {1, QObject::tr("Commercial information"), false, false}},
       {"WCOP", {2, QObject::tr("Copyright/Legal information"), true, false}},
       {"WOAF", {3, QObject::tr("Official audio file webpage"), true, false}},
       {"WOAR", {4, QObject::tr("Official artist/performer webpage"), false, false}},
       {"WOAS", {5, QObject::tr("Official audio source webpage"), true, false}},
       {"WORS", {6, QObject::tr("Official Internet radio station homepage"), true, false}},
       {"WPAY", {7, QObject::tr("Payment"), true, false}},
       {"WPUB", {8, QObject::tr("Publishers official webpage"), true, false}}};

optional<pair<QString const, UrlTypeSelector::TypeDescription> const *>search(uint type) {
    for(auto const &t : types) {
        if(t.second.type == type)
            return &t;
    }
    return nullopt;
}

} // namespace

UrlTypeSelector::UrlTypeSelector()
    : core::Selector() {
    allowUserValues(false);
    allowInsertion(false);
    allowDeletion(false);
    setUserValueId(userValueType);
    for (auto t : types)
        insertInit({t.second.type, t.second.description});
}

UrlTypeSelector *UrlTypeSelector::instance() {
    static UrlTypeSelector instance;
    return &instance;
}

uint UrlTypeSelector::typeOfId(QString const &id) const {
    if (types.contains(id))
        return types.at(id).type;
    assert(1 == 2 && "Unknown URL TagId");
}

QString UrlTypeSelector::descriptionOfType(uint i) const {
    auto v = search(i);
    if (v)
        return v.value()->second.description;
    return "";
}

QString UrlTypeSelector::idOfType(uint i) const {
    auto v = search(i);
    if (v)
        return v.value()->first;
    return "";
}

UrlTypeSelector::ConstTypeIterator UrlTypeSelector::begin() const {
    return types.cbegin();
}

UrlTypeSelector::ConstTypeIterator UrlTypeSelector::end() const {
    return types.cend();
}

} // namespace greu::id3
