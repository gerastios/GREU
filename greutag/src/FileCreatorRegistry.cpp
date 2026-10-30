#include "FileCreatorRegistry.h"
#include <QDir>
#include <QFileInfo>
#include <QLibrary>
#include "FileCreator.h"

using namespace std;

namespace greu::tag {

using namespace core;

FileCreatorRegistry &FileCreatorRegistry::instance() {
    static FileCreatorRegistry i;
    return i;
}

bool FileCreatorRegistry::checkAddonVersion(QLibrary &library,
                                            QString const &fileName,
                                            QString &error) {
    auto const versionFunction = reinterpret_cast<TagFileCreatorApiVersionFunction>(
        library.resolve(GREU_TAG_FILE_CREATOR_PLUGIN_API_VERSION_SYMBOL));
    auto const createFunction = reinterpret_cast<TagCreateFileCreatorFunction>(
        library.resolve(GREU_TAG_FILE_CREATOR_PLUGIN_CREATE_SYMBOL));
    if (!versionFunction || !createFunction) {
        error = QStringLiteral("'%1' is not a file creator plugin (missing entry points)")
                    .arg(fileName);
        return false;
    }

    int const version = versionFunction();
    if (version != GREU_TAG_FILE_CREATOR_PLUGIN_API_VERSION) {
        error = QStringLiteral("'%1' uses plugin API version %2, expected %3")
                    .arg(fileName)
                    .arg(version)
                    .arg(GREU_TAG_FILE_CREATOR_PLUGIN_API_VERSION);
        return false;
    }
    return true;
}

FileCreator *FileCreatorRegistry::createInterface(QLibrary &library) {
    auto const createFunction = reinterpret_cast<TagCreateFileCreatorFunction>(
        library.resolve(GREU_TAG_FILE_CREATOR_PLUGIN_CREATE_SYMBOL));
    return createFunction();
}

} // namespace greu::tag
