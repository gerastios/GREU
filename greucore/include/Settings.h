#pragma once

#include <QSettings>
#include "greuGlobal.h"

namespace greu::core {

class GREU_PUBLIC(CORE) Settings : public QSettings {
public:
    inline Settings(QString const &program)
        : QSettings(QStringLiteral("greu.de"), program) {}
};

} // namespace greu::core
