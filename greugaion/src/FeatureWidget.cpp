#include "FeatureWidget.h"
#include <QVBoxLayout>
#include <QWidget>
#include "FeatureModel.h"
#include "HeadWidget.h"
#include "MovieObjectItem.h"
#include "MovieObjectWidget.h"
#include "PlayableFileWidgets.h"
#include "greucore/include/GuardedPtr.h"

using namespace std;

namespace greu::gaion {

using namespace core;

struct FeatureWidget::Detail {
    GuardedPtr<QWidget> widget;
    GuardedPtr<QVBoxLayout> layout;
    unique_ptr<HeadWidget> head;
    unique_ptr<PlayableFileWidgets> playWidgets;

    void setup(PresentationWidget *parent,
               FeatureModel const *model,
               bool addLink,
               QString const &typeString,
               bool addSpecific) {
        widget = new QWidget;
        layout = new QVBoxLayout;
        widget->setLayout(layout.get());
        head = make_unique<HeadWidget>(parent, model, addLink, typeString, addSpecific);
        layout->addWidget(head->widget());
        auto file = model->file();
        if (file)
            playWidgets = make_unique<PlayableFileWidgets>(file, layout.get());
    }
};

FeatureWidget::FeatureWidget(
    PresentationWidget *p, FeatureModel const *m, bool al, QString const &ts, bool as)
    : d(GREU_MKDET()) {
    d->setup(p, m, al, ts, as);
}

QWidget *FeatureWidget::widget() const {
    return d->widget.get();
}

} // namespace greu::gaion
