#include "PlayerWindow.h"
#include <QTimer>
#include "greucore/include/Timer.h"
#include <greuui/include/VanishingCursor.h>

using namespace std;
using namespace std::chrono;

namespace greu::mpv {

using namespace ui;
using namespace core;

struct PlayerWindow::Detail {
    std::unique_ptr<VanishingCursor> cursor;
    Timer timer{milliseconds(10000)};
    ScopedConnection conn;
};

PlayerWindow::PlayerWindow()
    : d(GREU_MKDET()) {
    d->cursor = make_unique<VanishingCursor>(this);
    d->conn = d->timer.timeout().connect([this] { d->cursor->hide(); });
    d->timer.start();
}

void PlayerWindow::showCursor() {
    d->cursor->show();
    d->timer.stop();
    d->timer.start();
}

void PlayerWindow::mouseMoveEvent(QMouseEvent *) {
    showCursor();
}

} // namespace greu::mpv
