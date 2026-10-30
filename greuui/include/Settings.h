#pragma once
#include "greucore/include/Settings.h"

class QWidget;

namespace greu::ui {

class GREU_PUBLIC(UI) Settings : public core::Settings {
public:
    static Settings &instance();

    void save(QWidget const *widget);
    void restore(QWidget *widget);

private:
    Settings();
};

} // namespace greu::ui
