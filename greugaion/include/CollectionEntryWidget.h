#pragma once
#include "greucore/include/DetailPtr.h"

class QWidget;

namespace greu::movies {
class File;
}

namespace greu::gaion {

struct FeatureModel;
class PresentationWidget;

class GREU_PUBLIC(GAION) CollectionEntryWidget {
public:
    struct Entry {
        std::unique_ptr<CollectionEntryWidget> widget;
        std::unique_ptr<FeatureModel> featureModel;
    };
    using Entries = std::vector<Entry>;
    CollectionEntryWidget(FeatureModel const *,
                          PresentationWidget *,
                          QString const &typeString = {});
    QWidget *widget() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion
