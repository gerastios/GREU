#include "MovieObjectWidget.h"
#include <QScrollArea>
#include <QScrollBar>
#include <QVBoxLayout>
#include "FeatureWidget.h"
#include "MovieObjectItem.h"

using namespace std;

namespace greu::gaion {

using namespace core;

struct MovieObjectWidget::Detail {
    MovieObjectItem const *const item;
    GuardedPtr<QScrollArea> scroll;
    GuardedPtr<QWidget> widget;
    GuardedPtr<QVBoxLayout> layout;
    unique_ptr<FeatureWidget> featurWidget;

    Detail(MovieObjectItem const *i)
        : item(i) {}

    void init(MovieObjectWidget *that) {
        scroll = new QScrollArea;
        scroll->setWidgetResizable(true);
        widget = new QWidget(scroll.get());
        layout = new QVBoxLayout;
        widget->setLayout(layout.get());
        widget->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        featurWidget = make_unique<FeatureWidget>(that, item->featureModel());
        layout->addWidget(featurWidget->widget());
        that->addAfterHead(layout.get());
        scroll->setWidget(widget.get());
    }
};

MovieObjectWidget::MovieObjectWidget(MovieObjectItem const *i)
    : d(GREU_MKDET(i)) {}

QWidget *MovieObjectWidget::widget() const {
    return d->scroll.get();
}

MovieObjectItem const *MovieObjectWidget::item() const {
    return d->item;
}

using ScrollPos = pair<int, int>;

void MovieObjectWidget::setPosition(any const &v) {
    auto const &pos = any_cast<ScrollPos>(v);
    ;
    d->scroll->verticalScrollBar()->setValue(pos.first);
    d->scroll->horizontalScrollBar()->setValue(pos.second);
}

any MovieObjectWidget::position() const {
    ScrollPos res;
    res.first = d->scroll->verticalScrollBar()->value();
    res.second = d->scroll->horizontalScrollBar()->value();
    return res;
}

void MovieObjectWidget::init() {
    d->init(this);
}

} // namespace greu::gaion
