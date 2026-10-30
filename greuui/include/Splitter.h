#pragma once
#include <QSplitter>
#include "greucore/include/greuGlobal.h"

namespace greu::ui {

class GREU_PUBLIC(UI) Splitter : public QSplitter {
public:
    Splitter(Qt::Orientation, QString const &name);
};

} // namespace greu::ui
