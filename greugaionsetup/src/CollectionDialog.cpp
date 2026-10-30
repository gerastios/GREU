#include "CollectionDialog.h"
#include "greumovies/include/Collection.h"
#include "greumovies/include/CollectionsTable.h"

using namespace greu::gaion;
using namespace greu;
using namespace std;
using namespace ui;

namespace {

ComboInput::ComboValues collectionTexts() {
    ComboInput::ComboValues res;
    auto colls = movies::CollectionsTable::instance()->allCollectionIds();
    for (auto &c : colls)
        res.push_back({movies::Collection::collection(c)->nameInCollection(), c});
    ranges::sort(res, [](auto &f, auto &s) { return f.text < s.text; });
    return res;
}

} // namespace

namespace greu::gaion::setup {

CollectionDialog::CollectionDialog()
    : ComboInput(QObject::tr("Select Collection"), collectionTexts()) {}

movies::Collection *CollectionDialog::collection() const {
    return movies::Collection::collection(result());
}

} // namespace greu::gaion::setup
