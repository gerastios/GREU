#include "MainWindow.h"
#include <QPushButton>
#include <QScrollArea>
#include <QSplitter>
#include <QVBoxLayout>
#include <QWidget>
#include "SearchDialog.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/signalUtils.h"
#include "greucore/include/utils.h" // IWYU pragma: keep
#include "greugaion/include/NavigationController.h"
#include "greugaion/include/PresentationWidget.h"
#include "greugaionapp/include/Dispatcher.h"
#include "greumovies/include/SearchOptions.h" // IWYU pragma: keep
#include "greumovies/include/Settings.h"
#include "greuui/include/Application.h"
#include "greuui/include/Icons.h"
#include "greuui/include/Separator.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::gaion::app {

using namespace core;
using namespace ui;
using namespace movies;

struct MainWindow::Detail {
    GuardedPtr<QWidget> mainWidget;
    PresentationWidget *mediumWidget = nullptr;
    GuardedPtr<QVBoxLayout> mainLayout;
    GuardedPtr<QHBoxLayout> toolsLayout;
    GuardedPtr<QPushButton> movies, series, movieCollections, moviesByGenre, seriesByGenre,
        collections, back, forward, search, exit;
    GuardedPtr<Separator> sep1, sep2;
    ScopedConnection connNavigation;

    void setupUi(MainWindow *that) {
        mainWidget = new QWidget;
        mainWidget->setObjectName("mainWidget");
        mainLayout = new QVBoxLayout;
        mainWidget->setLayout(mainLayout.get());
        buildTools();
        that->setCentralWidget(mainWidget.get());
        connNavigation = GREU_NAVC->currentSet().connect([this] { activateNavigation(); });
        activateNavigation();
    }

    void activateNavigation() {
        back->setEnabled(GREU_NAVC->backwardPossible());
        forward->setEnabled(GREU_NAVC->forwardPossible());
    }

    void buildTools() {
        toolsLayout = new QHBoxLayout;
        mainLayout->addLayout(toolsLayout.get());
        sep1 = new Separator;
        sep2 = new Separator;
        addNewWidget(movies, QObject::tr("Movies"), toolsLayout.get());
        ui::connect(movies.get(), [] { GREU_D->showMovies(); });
        addNewWidget(series, QObject::tr("TV Series"), toolsLayout.get());
        ui::connect(series.get(), [] { GREU_D->showSeries(); });
        addNewWidget(movieCollections, QObject::tr("Movie collections"), toolsLayout.get());
        ui::connect(movieCollections.get(), [] { GREU_D->showMovieCollections(); });
        addNewWidget(collections, QObject::tr("Collections"), toolsLayout.get());
        ui::connect(collections.get(), [] { GREU_D->showCollections(); });
        addNewWidget(moviesByGenre, QObject::tr("Movies by genre"), toolsLayout.get());
        ui::connect(moviesByGenre.get(), [] { GREU_D->showMoviesByGenre(); });
        addNewWidget(seriesByGenre, QObject::tr("Series by genre"), toolsLayout.get());
        ui::connect(seriesByGenre.get(), [] { GREU_D->showSeriesByGenre(); });

        toolsLayout->addWidget(sep1.get());

        addNewWidget(back, GREU_ICON(back), "", toolsLayout.get());
        ui::connect(back.get(), [] { GREU_D->back(); });
        addNewWidget(forward, GREU_ICON(forward), "", toolsLayout.get());
        ui::connect(forward.get(), [] { GREU_D->forward(); });

        toolsLayout->addWidget(sep2.get());

        addNewWidget(search, GREU_ICON(search), "", toolsLayout.get());
        ui::connect(search.get(), [] {
            SearchDialog d;
            if (d.exec())
                GREU_D->search(d.options());
        });

        toolsLayout->addWidget(sep2.get());

        addNewWidget(exit, GREU_ICON(close), "", toolsLayout.get());
        ui::connect(exit.get(), [] { GREU_D->exit(); });

        toolsLayout->addStretch(10);
    }
};

MainWindow::MainWindow()
    : d(GREU_MKDET()) {
    setObjectName("mainwindowapp");
    setWindowTitle((Application::instance()->winTitle() + ": %1")
                       .arg(movies::Settings::instance()->pathPrefix()));
}

void MainWindow::setMediumWidget(PresentationWidget *w) {
    if (d->mediumWidget) {
        d->mainLayout->removeWidget(d->mediumWidget->widget());
        d->mediumWidget->widget()->hide();
    }
    d->mediumWidget = w;
    if (d->mediumWidget)
        d->mainLayout->addWidget(d->mediumWidget->widget());
}

PresentationWidget *MainWindow::mediumWidget() const {
    return d->mediumWidget;
}

void MainWindow::setupUiInt() {
    d->setupUi(this);
}

} // namespace greu::gaion::app
