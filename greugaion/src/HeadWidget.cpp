#include "HeadWidget.h"
#include <QHBoxLayout>
#include "Dispatcher.h"
#include "FeatureModel.h"
#include "HeadInfoWidget.h"
#include "MovieObjectItem.h"
#include "MovieObjectWidget.h"
#include "Styles.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/signalUtils.h"
#include "greucore/include/utils.h"
#include "greuui/include/ImageButton.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;

struct HeadWidget::Detail {
    GuardedPtr<QWidget> widget;
    GuardedPtr<QHBoxLayout> layout;
    unique_ptr<HeadInfoWidget> right;
    GuardedPtr<ImageLabel> image;
    ScopedConnection imageConn;

    void setup(PresentationWidget *parent,
               FeatureModel const *model,
               bool addLink,
               QString const &typeString,
               bool addSpecific) {
        widget = new QWidget;
        layout = new QHBoxLayout;
        widget->setLayout(layout.get());
        if (addLink) {
            image = new ImageButton;
            imageConn = safeCast<ImageButton>(image.get())->clicked().connect([](QString const &s) {
                GREU_D->linkClicked(s);
            });
        } else {
            image = new ImageLabel;
        }
        layout->addWidget(image.get());
        image->setImage(model->image().scaledToWidth(GREU_STYL->bigImageWidth(),
                                                     Qt::TransformationMode::SmoothTransformation));

        image->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        right = make_unique<HeadInfoWidget>(parent, model, addLink, typeString, addSpecific);
        right->widget()->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        layout->setStretch(0, 0);
        layout->setStretch(1, 1);
        layout->addWidget(right->widget());
    }
};

HeadWidget::HeadWidget(
    PresentationWidget *p, FeatureModel const *m, bool al, QString const &ts, bool as)
    : d(GREU_MKDET()) {
    d->setup(p, m, al, ts, as);
}

QWidget *HeadWidget::widget() const {
    return d->widget.get();
}

} // namespace greu::gaion
