#include "ListItem.h"
#include <QJsonObject>
#include "ListWidget.h"
#include "MovieObjectItem.h" // IWYU pragma: keep

using namespace std;

namespace greu::gaion {

using namespace core;

struct ListItem::Detail {
    unique_ptr<ListWidget> widget;
};

ListItem::ListItem(Presentation const &p)
    : PresentationItem(p)
    , d(GREU_MKDET()) {
}

PresentationWidget *ListItem::widget() const {
    if(!d->widget)
        d->widget = make_unique<ListWidget>(this);
    return d->widget.get();
}

} // namespace greu::gaion
