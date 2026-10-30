#pragma once
#include <QSplashScreen>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) SplashScreen : public QSplashScreen {
public:
    SplashScreen(QPixmap const &, Qt::WindowFlags = Qt::WindowFlags());

    void setPercent(int);
    virtual void drawContents(QPainter *painter) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
