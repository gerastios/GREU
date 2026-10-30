#pragma once
#include "greucore/include/DetailPtr.h"

namespace greu::core {
class Log;
}

class QWidget;

namespace greu::ui {

class GREU_PUBLIC(UI) LogViewer {
public:
    explicit LogViewer(core::Log *log);

    QWidget *setupUi();
    void clear();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
