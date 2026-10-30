#include "Separator.h"

namespace greu::ui {

Separator::Separator(Orientation o) {
    if (o == vertical)
        setFrameShape(QFrame::VLine);
    else
        setFrameShape(QFrame::HLine);
    setFrameShadow(QFrame::Sunken);
}

} // namespace greu::ui
