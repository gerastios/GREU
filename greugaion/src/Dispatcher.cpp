#include "Dispatcher.h"
#include <QPixmap>
#include "LinkController.h"
#include "MainWindow.h"
#include "NavigationController.h"
#include "Player.h"
#include "PresentationItem.h"
#include "PresentationWidget.h"
#include "Styles.h"
#include "greucore/include/utils.h" // IWYU pragma: keep
#include "greuui/include/Application.h"
#include "greuui/include/MainWindow.h"

using namespace std;
using namespace greu::core;
using namespace greu::ui;
using namespace greu::movies;

namespace greu::gaion {

struct Dispatcher::Detail {
    unique_ptr<Player> player;
    FileSig playingFinished;
    ScopedConnection connFinished, connNavigation;
};

Dispatcher::Dispatcher()
    : d(GREU_MKDET()) {
    ui::Application::instance()->setStyleSheet(
        QString("QPushButton { font-size: %1px; }").arg(GREU_STYL->width() / 137));
    d->connNavigation = GREU_NAVC->currentSet().connect([] {
        auto ci = GREU_NAVC->currentItem();
        GREU_M->setMediumWidget((ci ? ci->widget() : nullptr));
    });
}

void Dispatcher::back() {
    if (GREU_NAVC->backwardPossible()) {
        GREU_NAVC->backward();
    }
}

void Dispatcher::forward() {
    if (GREU_NAVC->forwardPossible()) {
        GREU_NAVC->forward();
    }
}

Dispatcher::FileSig &Dispatcher::playingFinished() const {
    return d->playingFinished;
}

void Dispatcher::play(File *file, PlayType type) {
    if (!file)
        return;
    d->player.reset(new Player(file));
    d->connFinished = d->player->finished().connect([this, file] { d->playingFinished(file); });
    switch (type) {
    case PlayType::play:
        d->player->play();
        break;
    case PlayType::continuePlaying:
        d->player->playContinue();
        break;
    case PlayType::playExtended:
        d->player->playExtended();
        break;
    }
}

void Dispatcher::exit() {
    mainWindow()->close();
}

void greu::gaion::Dispatcher::linkClicked(QString const &s) {
    GREU_NAVC->setCurrent(GREU_M->mediumWidget()->linkController()->presentation(s));
}

} // namespace greu::gaion
