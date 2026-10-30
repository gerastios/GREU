#include "Slider.h"
#include <QMouseEvent>

namespace greu::ui {

using namespace core;

struct Slider::Detail {
    MouseEventSig mousePressed, mouseReleased;
};

Slider::Slider(Qt::Orientation orientation)
    : QSlider(orientation)
    , d(GREU_MKDET()) {}

MouseEventSig &Slider::mousePressed() const {
    return d->mousePressed;
}

MouseEventSig &Slider::mouseReleased() const {
    return d->mouseReleased;
}

void Slider::mousePressEvent(QMouseEvent *event) {
    d->mousePressed(event);
    QSlider::mousePressEvent(event);
}

void Slider::mouseReleaseEvent(QMouseEvent *event) {
    d->mouseReleased(event);
    QSlider::mouseReleaseEvent(event);
}

} // namespace greu::ui
