#include "Collectable.h"
#include <QObject>

namespace greu::movies {

QString Collectable::typeString(Type t) {
    switch (t) {
    case Type::collection:
        return QObject::tr("Collection");
    case Type::movieCollection:
        return QObject::tr("Movie Collection");
    case Type::movie:
        return QObject::tr("Movie");
    case Type::episode:
        return QObject::tr("Episode");
    case Type::series:
        return QObject::tr("Series");
    }
    return {};
}

} // namespace greu::movies
