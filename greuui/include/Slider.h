#pragma once
#include <QSlider>
#include "greucore/include/DetailPtr.h"
#include "greucore/include/signalUtils.h"

namespace greu::ui {

class GREU_PUBLIC(UI) Slider : public QSlider {
public:
    Slider(Qt::Orientation = Qt::Orientation::Vertical);

    core::MouseEventSig &mousePressed() const;
    core::MouseEventSig &mouseReleased() const;

protected:
    virtual void mousePressEvent(QMouseEvent *) override;
    virtual void mouseReleaseEvent(QMouseEvent *) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
