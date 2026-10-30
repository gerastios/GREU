#include "Application.h"
#include <QCommandLineParser>
#include <QDir>
#include <QFileInfo>
#include "Dispatcher.h"

namespace {
QString const name{"grylizo"};
QString const version{"0.9.1"};
QString const title{"Γρυλίζω"};
} // namespace

namespace greu::grylizo {

Application::Application(int &argc, char **argv)
    : ui::Application(argc, argv) {
    setApplicationName(name);
    setApplicationVersion(version);
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

void Application::parseCommandLine(Dispatcher *dispatcher) {
    QCommandLineParser parser;
    parser.setApplicationDescription(QObject::tr("Tag audio files"));
    parser.addHelpOption();
    parser.addVersionOption();
    parser.addPositionalArgument("audioDir",
                                 QApplication::translate("main", "Audio file directory."));
    QCommandLineOption packsz{{"p", "packsize"},
                              QApplication::translate("main", "Package size for file analyzing."),
                              "pack"};
    parser.addOption(packsz);
    QCommandLineOption msgsz{{"m", "msgsize"},
                             QApplication::translate("main", "Message size for file analyzing."),
                             "mess"};
    parser.addOption(msgsz);
    QCommandLineOption noSplash{{"n", "nosplash"},
                                QApplication::translate("main", "Do not display splashscreen.")};
    parser.addOption(noSplash);
    parser.process(*this);
    auto posArgs = parser.positionalArguments();
    if (posArgs.size() > 0) {
        QFileInfo info{posArgs[0]};
        if (info.isDir())
            dispatcher->setAudioPath(info.filePath());
        else if (info.isFile())
            dispatcher->setAudioPath(info.dir().path());
    }
    bool ok;
    {
        auto s = parser.value(packsz);
        auto p = s.toUInt(&ok);
        if (ok)
            dispatcher->setPackageSize(p);
    }
    {
        auto s = parser.value(msgsz);
        auto p = s.toUInt(&ok);
        if (ok)
            dispatcher->setMessageSize(p);
    }
    if (parser.isSet(noSplash))
        dispatcher->setNoSplash();
}

} // namespace greu::grylizo
