#pragma once

#include <QString>
#include "greucore/include/AddonRegistry.h"

class QFileInfo;

namespace greu::tag {

class FileCreator;

class GREU_PUBLIC(TAG) FileCreatorRegistry: public core::AddonRegistry<FileCreator, QFileInfo> {
public:
    static FileCreatorRegistry &instance();

private:
    FileCreatorRegistry() = default;
    virtual bool checkAddonVersion(QLibrary &, QString const &fileName, QString &error) override;
    virtual FileCreator *createInterface(QLibrary &) override;

};

} // namespace greu::tag
