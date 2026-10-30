#include "Splitter.h"

namespace greu::ui {

Splitter::Splitter(Qt::Orientation orientation, QString const &name)
    : QSplitter(orientation) {
    setObjectName(name);
    setStyleSheet("QSplitter::handle{background: gray}");
}

} // namespace greu::ui
