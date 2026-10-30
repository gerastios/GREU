#include "dvdInfo.h"
#include <QProcess>

namespace greu::geranos {

QString dvdInfo(QString const &filePath) {
    QProcess p;
    p.start("/usr/bin/lsdvd", {{"-a"}, {"-s"}, {filePath}});
    p.waitForFinished(-1);
    return p.readAllStandardOutput();
}

} // namespace greu::geranos
