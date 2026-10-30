#include "Application.h"
#include <QCommandLineParser>
#include "greucore/include/Language.h"
#include "greucore/include/utils.h"
#include "greugaionapp/include/Dispatcher.h"
#include "greugaionsetup/include/Dispatcher.h"
#include "greumovies/include/Settings.h"
#include "greumovies/include/database.h"
#include "greusql/include/Table.h"
#include "greutmdb/include/TheMovieDatabase.h"
#include <iostream>

using namespace greu::core;
using namespace greu::movies;
using namespace std;

namespace {
QString const name{"gaion"};
QString const version{"0.2.0"};
QString const title{"Γαίων"};
QString schemaToInit{""};
QString passwordInit("");
QString schemaToPlay{""};
QString pathPrefix{""};
QString schemaToSetup{""};
QString language{""};
bool noSplash = false;

void parseCommandLine(Application *that) {
    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Movie collection manager"));
    parser.addHelpOption();
    parser.addVersionOption();
    QCommandLineOption playSchema{{"p", "play"},
                                  QApplication::translate("main", "Play schema."),
                                  "schema"};
    parser.addOption(playSchema);
    QCommandLineOption initSchema{{"i", "initschema"},
                                  QApplication::translate("main", "Initialize schema."),
                                  "init"};
    parser.addOption(initSchema);
    QCommandLineOption prefix{{"x", "pathprefix"},
                              QApplication::translate("main", "Path prefix."),
                              "schema"};
    parser.addOption(prefix);
    QCommandLineOption initPassword{{"r", "rootpwd"},
                                    QApplication::translate("main", "Root password."),
                                    "init"};
    parser.addOption(initPassword);
    QCommandLineOption setupSchema{{"s", "setupschema"},
                                   QApplication::translate("main", "Setup schema."),
                                   "setup"};
    parser.addOption(setupSchema);
    QCommandLineOption setupLanguage{{"l", "setuplanguage"},
                                     QApplication::translate("main", "Setup language."),
                                     "setup"};
    parser.addOption(setupLanguage);
    QCommandLineOption noSplashOpt{{"n", "nosplash"},
                                   QApplication::translate("main", "No splash."),
                                   "schema"};
    parser.addOption(noSplashOpt);
    parser.process(*that);
    schemaToPlay = parser.value(playSchema);
    schemaToInit = parser.value(initSchema);
    pathPrefix = parser.value(prefix);
    schemaToSetup = parser.value(setupSchema);
    language = parser.value(setupLanguage);
    passwordInit = parser.value(initPassword);
    ::noSplash = parser.isSet(noSplashOpt);
}

} // namespace

struct Application::Detail {
    unique_ptr<greu::gaion::Dispatcher> dispatcher;
    unique_ptr<greu::tmdb::TheMovieDatabase> tmdb;

    void prepareSetup(QString const &schema, QString const &languageCode) {
        auto lango = Language::fromCode(languageCode);
        if (!lango) {
            cout << QObject::tr("Unknown language code %1").arg(languageCode).toStdString() << endl;
            std::exit(8);
        }
        tmdb = make_unique<greu::tmdb::TheMovieDatabase>(*lango.value(),
                                                         *Language::fromCode2(
                                                              Language::Code2::English)
                                                              .value());
        openDatabase(schema, true);
        greu::sql::Table::setReadOnly(false);
        greu::movies::Settings::instance(false)->setLanguage(*lango.value());
        greu::movies::Settings::instance(false)->setPathPrefix(pathPrefix);
        dispatcher = make_unique<greu::gaion::setup::Dispatcher>();
        dispatcher->initialize();
    }

    void preparePlay(QString const &schema) {
        openDatabase(schema);
        greu::sql::Table::setReadOnly(true);
        greu::movies::Settings::instance(true);
        greu::movies::Settings::instance()->setPathPrefix(pathPrefix);
        dispatcher = make_unique<greu::gaion::app::Dispatcher>();
        safeCast<greu::gaion::app::Dispatcher>(dispatcher.get())->setNoSplash(::noSplash);
        dispatcher->initialize();
    }
};

Application::Application(int &argc, char **argv)
    : greu::ui::Application(argc, argv)
    , d(GREU_MKDET()) {}

int Application::run() {
    parseCommandLine(this);
    if ((schemaToInit.isEmpty() && schemaToPlay.isEmpty() && schemaToSetup.isEmpty())
        || (!schemaToInit.isEmpty() && !schemaToPlay.isEmpty())
        || (!schemaToInit.isEmpty() && !schemaToSetup.isEmpty())
        || (!schemaToPlay.isEmpty() && !schemaToSetup.isEmpty())) {
        cout << QObject::tr("Select the play, the init or the setup option must be set").toStdString()
             << endl;
        std::exit(8);
    }
    if (!schemaToSetup.isEmpty() && language.isEmpty()) {
        cout << QObject::tr("For setup add the setup language").toStdString() << endl;
        std::exit(8);
    }
    if (!schemaToInit.isEmpty() && passwordInit.isEmpty()) {
        cout << QObject::tr("For init add the root password").toStdString() << endl;
        std::exit(8);
    }
    if (!schemaToSetup.isEmpty() && pathPrefix.isEmpty()) {
        cout << QObject::tr("For setup add the path prefix").toStdString() << endl;
        std::exit(8);
    }
    if (!schemaToPlay.isEmpty() && pathPrefix.isEmpty()) {
        cout << QObject::tr("For play add the path prefix").toStdString() << endl;
        std::exit(8);
    }
    if (!schemaToInit.isEmpty()) {
        initDatabase(schemaToInit, passwordInit);
        std::exit(0);
    } else if (!schemaToSetup.isEmpty())
        d->prepareSetup(schemaToSetup, language);
    else
        d->preparePlay(schemaToPlay);
    auto rc = exec();
    if (d->dispatcher)
        d->dispatcher->shutDown();
    return rc;
}

QString const &Application::appName() {
    return name;
}

QString const &Application::appVersion() {
    return version;
}

QString const &Application::winTitle() {
    return title;
}
