#pragma once

#include <QString>
#include "greucore/include/AddonRegistry.h"

class QFileInfo;

namespace greu::id3 {

class FrameHandler;

class GREU_PUBLIC(ID3) FrameHandlerRegistry: public core::AddonRegistry<FrameHandler, QString> {
public:
    static FrameHandlerRegistry &instance();
    bool notDiscardable(QString const &) const;

private:
    FrameHandlerRegistry();
    virtual bool checkAddonVersion(QLibrary &, QString const &fileName, QString &error) override;
    virtual FrameHandler *createInterface(QLibrary &) override;

};

} // namespace greu::tag
