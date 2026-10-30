#include "CollectionEntryWidget.h"
#include <QBoxLayout>
#include "FeatureWidget.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;

struct CollectionEntryWidget::Detail {
    unique_ptr<FeatureWidget> widget;

    Detail(FeatureModel const *featureModel, PresentationWidget *parent, QString const &typeString) {
        widget = make_unique<FeatureWidget>(parent, featureModel, true, typeString, false);
    }
};

CollectionEntryWidget::CollectionEntryWidget(FeatureModel const *fm,
                                             PresentationWidget *w,
                                             QString const &ts)
    : d(GREU_MKDET(fm, w, ts)) {}

QWidget *CollectionEntryWidget::widget() const {
    return d->widget->widget();
}

} // namespace greu::gaion
