#include "Icons.h"

using namespace std;
using namespace greu::ui;

#define ADD_ICON(_NAME) \
    case Icons::_NAME: \
        icon = make_unique<QIcon>(QString(":/images/images/%1.png").arg(#_NAME)); \
        icons.insert({name, std::move(icon)}); \
        break;

namespace {
static unordered_map<Icons::Name, unique_ptr<QIcon>> icons;
static mutex mtx;
} // namespace

namespace greu::ui {

QIcon const &Icons::icon(Name const &name) {
    lock_guard<mutex> guard(mtx);
    if (!icons.contains(name)) {
        unique_ptr<QIcon> icon;
        switch (name) { __GREU_META_ICON(ADD_ICON) }
    }
    return *icons[name];
}

} // namespace greu::ui

void qInitResources_images();
