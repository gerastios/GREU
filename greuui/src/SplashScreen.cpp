#include "SplashScreen.h"
#include <QProgressBar>
#include <QStyleOptionProgressBar>
#include "greucore/include/GuardedPtr.h"
#include <mutex>

using namespace std;

namespace {
mutex mtx;
}

namespace greu::ui {

using namespace core;

struct SplashScreen::Detail {
    GuardedPtr<QProgressBar> progress;

    void setPercent(int v, SplashScreen *that) {
        lock_guard<mutex> guard(mtx);
        progress->setValue(v);
        that->repaint();
    }
};

SplashScreen::SplashScreen(QPixmap const &pixmap, Qt::WindowFlags flags)
    : QSplashScreen(pixmap, flags)
    , d(GREU_MKDET()) {
    d->progress = new QProgressBar(this);
    setWindowFlag(Qt::WindowStaysOnTopHint);
    auto pixrect = this->pixmap().rect();
    d->progress->setGeometry(pixrect.x(), pixrect.y(), pixrect.width(), 5);
    d->progress->setValue(0);
    d->progress->setMinimum(0);
    d->progress->setMaximum(100);
    d->progress->setTextVisible(false);
    d->progress->setEnabled(true);
}

void SplashScreen::setPercent(int v) {
    d->setPercent(v, this);
}

void SplashScreen::drawContents(QPainter *painter) {
    QSplashScreen::drawContents(painter);
    d->progress->render(painter);
}

} // namespace greu::ui
