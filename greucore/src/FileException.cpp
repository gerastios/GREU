#include "FileException.h"
#include <QFileInfo>

using namespace std;

namespace greu::core {

struct FileException::Detail {
    QFileInfo const info;

    Detail(QFileInfo const &i)
        : info(i) {}
};

FileException::FileException(QString const &t, QFileInfo const &i)
    : Exception(t)
    , d(GREU_MKDET(i)) {}

QFileInfo const &FileException::info() const {
    return d->info;
}

QString FileException::text() const {
    return QString{"%1: %2"}.arg(Exception::text()).arg(d->info.fileName());
}

} // namespace greu::core
