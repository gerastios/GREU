#pragma once
#include "Exception.h"

class QFileInfo;

namespace greu::core {

class GREU_PUBLIC(CORE) FileException : public Exception {
public:
    FileException(QString const &, QFileInfo const &);

    QFileInfo const &info() const;
    virtual QString text() const override;

private:
    struct Detail;
    DetailPtr<Detail> d;
};

} // namespace greu::core
