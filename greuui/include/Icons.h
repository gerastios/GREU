#pragma once
#include <QIcon>
#include "greucore/include/greuGlobal.h"
#include "iconsMeta.h"

#define NAME(_NAME) _NAME,
//

namespace greu::ui {

class GREU_PUBLIC(UI) Icons {
public:
    enum Name { __GREU_META_ICON(NAME) };
    Icons() = delete;

public:
    static QIcon const &icon(Name const &name);
};

#define GREU_ICON(NAME) Icons::icon(Icons::NAME)
#undef NAME

} // namespace greu::ui
