#include "Actions.h"
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QToolBar>
#include "CollectionEditor.h"
#include "Dispatcher.h"
#include "MainWindow.h"
#include "SettingsPopup.h"
#include "greucore/include/Exception.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/utils.h" // IWYU pragma: keep
#include "greugaion/include/NavigationController.h"
#include "greumovies/include/Settings.h"
#include "greuui/include/Action.h"
#include "greuui/include/Application.h"
#include "greuui/include/Icons.h"
#include "greuui/include/connect.h"

#define PRIV(A) GuardedPtr<Action> A

#define IMP(A) \
    Action &Actions::A() const { \
        return *d->A; \
    }

using namespace std;
using namespace greu::core;
using namespace greu::ui;
using namespace greu::movies;

namespace greu::gaion::setup {

struct Actions::Detail {
    GREU_GAION_ACTION_SETUP_META(PRIV)
    StringSig basePathChanged;
    GuardedPtr<QMenuBar> menu;
    GuardedPtr<QMenu> fileMenu, editMenu, settingMenu, helpMenu;
    GuardedPtr<QToolBar> toolBar;
    ScopedConnection connNavigation;

    Detail()
        : about(new Action(QObject::tr("About..."),
                           QKeySequence(Qt::CTRL | Qt::Key_H),
                           GREU_ICON(about)))
        , back(
              new Action(QObject::tr("Back"), QKeySequence(Qt::ALT | Qt::Key_Left), GREU_ICON(back)))
        , exit(new Action(QObject::tr("Exit"), QKeySequence(Qt::CTRL | Qt::Key_Q), GREU_ICON(close)))
        , objectSetImage(new Action(QObject::tr("Set image..."),
                                    QKeySequence(Qt::ALT | Qt::Key_I),
                                    GREU_ICON(image)))
        , createCollection(new Action(QObject::tr("Create collection...")))
        , deleteCollection(new Action(QObject::tr("Create collection...")))
        , addToCollection(new Action(QObject::tr("Add to collection...")))
        , removeFromCollection(new Action(QObject::tr("Remove from collection...")))
        , forward(new Action(QObject::tr("Forward"),
                             QKeySequence(Qt::ALT | Qt::Key_Right),
                             GREU_ICON(forward)))
        , playFile(new Action(QObject::tr("Play file..."),
                              QKeySequence(Qt::CTRL | Qt::Key_P),
                              GREU_ICON(play)))
        , search(new Action(QObject::tr("Search..."),
                            QKeySequence(Qt::ALT | Qt::Key_S),
                            GREU_ICON(search)))
        , settings(new Action(QObject::tr("Settings..."),
                              QKeySequence(Qt::ALT | Qt::Key_T),
                              GREU_ICON(settings)))
        , updateAll(new Action(QObject::tr("Update all"),
                               QKeySequence(Qt::ALT | Qt::CTRL | Qt::Key_U),
                               GREU_ICON(update)))
        , updateFile(new Action(QObject::tr("Update file"), QKeySequence(Qt::CTRL | Qt::Key_U)))
        , updateObject(new Action(QObject::tr("Update object"), QKeySequence(Qt::CTRL | Qt::Key_O)))
        , menu(new QMenuBar)
        , fileMenu(new QMenu(QObject::tr("&File")))
        , editMenu(new QMenu(QObject::tr("&Edit")))
        , settingMenu(new QMenu(QObject::tr("&Settings")))
        , helpMenu(new QMenu(QObject::tr("&Help")))
        , toolBar(new QToolBar) {
        ui::connect(objectSetImage.get(), [] { GREU_D->setImageForObject(); });
        ui::connect(about.get(), [] {
            QMessageBox::information(
                nullptr,
                QObject::tr("About Gain"),
                QObject::tr("Gaion Video Manager %1\n© 2024-2025 Gerd Rother\ngerd.rother@gmx.net")
                    .arg(Application::instance()->appVersion()),
                QMessageBox::Button::Ok);
            ;
        });
        ui::connect(settings.get(), [this] {
            try {
                auto oldPath = movies::Settings::instance()->pathPrefix();
                if (SettingsPopup{}.exec()) {
                    auto newPath = movies::Settings::instance()->pathPrefix();
                    if (newPath != oldPath) {
                        basePathChanged(newPath);
                    }
                }
            } catch (Exception &e) {
                QMessageBox::critical(nullptr,
                                      QObject::tr("Error"),
                                      e.text(),
                                      QMessageBox::Button::Ok);
            }
        });
        ui::connect(back.get(), [] { GREU_D->back(); });
        ui::connect(forward.get(), [] { GREU_D->forward(); });
        ui::connect(exit.get(), [] { GREU_D->exit(); });
        ui::connect(createCollection.get(), [] {
            GuardedPtr<CollectionEditor> editor = new CollectionEditor;
            if (editor->exec()) {
                auto coll = Object::object<Collection>(Uuid{});
                coll->setNew();
                coll->setName(editor->text());
                coll->setDescription(editor->description());
                auto i = editor->image();
                coll->setImageX(i);
                coll->setSortOrder(editor->sortOrder());
                coll->save();
            }
        });
        ui::connect(deleteCollection.get(), [] { GREU_D->deleteCollection(); });
        ui::connect(addToCollection.get(), [] { GREU_D->addMediumToCollection(); });
        ui::connect(removeFromCollection.get(), [] { GREU_D->removeMediumFromCollection(); });
        ui::connect(updateObject.get(), [] { GREU_D->updateObject(); });

        menu->addMenu(fileMenu.get());
        menu->addMenu(editMenu.get());
        menu->addMenu(settingMenu.get());
        menu->addMenu(helpMenu.get());
        fileMenu->addAction(playFile.get());
        fileMenu->addAction(search.get());
        fileMenu->addSeparator();
        fileMenu->addAction(exit.get());
        editMenu->addAction(objectSetImage.get());
        editMenu->addAction(createCollection.get());
        editMenu->addAction(addToCollection.get());
        editMenu->addAction(removeFromCollection.get());
        editMenu->addAction(updateObject.get());
        settingMenu->addAction(settings.get());
        settingMenu->addAction(updateFile.get());
        settingMenu->addAction(updateAll.get());
        helpMenu->addAction(about.get());
        toolBar->setObjectName("setupmainwintoolbar");
        toolBar->addAction(back.get());
        toolBar->addAction(forward.get());
        toolBar->addSeparator();
        toolBar->addAction(playFile.get());
        toolBar->addAction(search.get());
        toolBar->addSeparator();
        toolBar->addAction(updateAll.get());
        toolBar->addAction(updateObject.get());
        toolBar->addAction(settings.get());
        GREU_M->setMenuBar(menu.get());
        GREU_M->addToolBar(toolBar.get());

        connNavigation = GREU_NAVC->currentSet().connect([this] { activateNavigation(); });
        activateNavigation();
    }

    void activateNavigation() {
        back->setEnabled(GREU_NAVC->backwardPossible());
        forward->setEnabled(GREU_NAVC->forwardPossible());
    }

};

Actions::Actions()
    : d(GREU_MKDET()) {}

StringSig &Actions::basePathChanged() const {
    return d->basePathChanged;
}

GREU_GAION_ACTION_SETUP_META(IMP)

} // namespace greu::gaion::setup
