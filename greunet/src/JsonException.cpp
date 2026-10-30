#include "JsonException.h"
#include <QString>

namespace greu::net {

JsonException::JsonException(QJsonParseError const &error)
    : Exception(error.errorString()) {}

} // namespace greu::net
