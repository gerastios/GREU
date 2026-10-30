#include "Exception.h"
#include <QString>

namespace greu::core {

struct Exception::Detail {
    QString const text;
    Detail(QStringView const &t)
        : text(t) {}
};

Exception::Exception(QString const &t)
    : d(GREU_MKDET(t)) {}

QString Exception::text() const {
    return d->text;
}

} // namespace greu::core
