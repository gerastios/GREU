#include "LabelledSlider.h"
#include <QLabel>
#include <QVBoxLayout>
#include "Slider.h"
#include "greucore/include/GuardedPtr.h"
#include "utils.h"

namespace greu::ui {

using namespace core;

struct LabelledSlider::Detail {
    GuardedPtr<Slider> slider;
    GuardedPtr<QLabel> label;
    GuardedPtr<QVBoxLayout> layout;

    void init() {
        layout = new QVBoxLayout;
        addNewWidget(label, layout.get());
        addNewWidget(slider, Qt::Orientation::Horizontal, layout.get());
    }

    void setEnabled(bool enabled) {
        slider->setEnabled(enabled);
        label->setVisible(enabled);
    }
};

LabelledSlider::LabelledSlider()
    : d(GREU_MKDET()) {
    d->init();
    setLayout(d->layout.get());
}

Slider *LabelledSlider::slider() const {
    return d->slider.get();
}

QLabel *LabelledSlider::label() const {
    return d->label.get();
}

void LabelledSlider::setEnabled(bool enabled) {
    d->setEnabled(enabled);
}

} // namespace greu::ui
