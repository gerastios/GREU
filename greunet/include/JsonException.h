#pragma once
#include <QJsonParseError>
#include "greucore/include/Exception.h"

namespace greu::net {

class GREU_PUBLIC(NET) JsonException : public core::Exception {
public:
    JsonException(QJsonParseError const &);
};

} // namespace greu::net
