#pragma once

#include "DetailPtr.h"
#include "greuGlobal.h"
#include <exception>

class QStringView;
class QString;

namespace greu::core {

class GREU_PUBLIC(CORE) Exception : public std::exception {
public:
    Exception(QString const &);
    virtual QString text() const;

private:
    struct Detail;
    DetailPtr<Detail> d;
};

} // namespace greu::core
