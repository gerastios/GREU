#include "FrameHandlerRegistry.h"
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include "DefaultFrameHandler.h"
#include "FrameHandler.h"

using namespace std;

namespace {
greu::id3::DefaultFrameHandler defHandler;
QString const defName{"default"};
} // namespace

namespace greu::id3 {

using namespace core;

FrameHandlerRegistry &FrameHandlerRegistry::instance() {
    static FrameHandlerRegistry i;
    return i;
}

bool FrameHandlerRegistry::notDiscardable(QString const &n) const {
    return n == defName;
}

FrameHandlerRegistry::FrameHandlerRegistry() {
    registerAddon(&defHandler, defName);
}

bool FrameHandlerRegistry::checkAddonVersion(QLibrary &library,
                                             QString const &fileName,
                                             QString &error) {
    auto const versionFunction = reinterpret_cast<Id3FrameHandlerApiVersionFunction>(
        library.resolve(GREU_ID3_FRAME_HANDLER_PLUGIN_API_VERSION_SYMBOL));
    auto const createFunction = reinterpret_cast<Id3FrameHandlerFunction>(
        library.resolve(GREU_ID3_FRAME_HANDLER_PLUGIN_CREATE_SYMBOL));
    if (!versionFunction || !createFunction) {
        error = QStringLiteral("'%1' is not a frame handler plugin (missing entry points)")
                    .arg(fileName);
        return false;
    }

    int const version = versionFunction();
    if (version != GREU_ID3_FRAME_HANDLER_PLUGIN_API_VERSION) {
        error = QStringLiteral("'%1' uses plugin API version %2, expected %3")
                    .arg(fileName)
                    .arg(version)
                    .arg(GREU_ID3_FRAME_HANDLER_PLUGIN_API_VERSION);
        return false;
    }
    return true;
}

FrameHandler *FrameHandlerRegistry::createInterface(QLibrary &library) {
    auto const createFunction = reinterpret_cast<Id3FrameHandlerFunction>(
        library.resolve(GREU_ID3_FRAME_HANDLER_PLUGIN_CREATE_SYMBOL));
    return createFunction();
}

} // namespace greu::id3
