#pragma once
#include <QDialog>
#include "greucore/include/DetailPtr.h"

namespace greu::mpv {

class GREU_PUBLIC(MPV) PlayerWindow: public QDialog {
public:
    PlayerWindow();
    void showCursor();

protected:
    virtual void mouseMoveEvent(QMouseEvent *event) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};
}
