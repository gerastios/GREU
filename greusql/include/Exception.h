#pragma once
#include "greucore/include/Exception.h"

class QSqlError;

namespace greu::sql {

class Query;

class GREU_PUBLIC(SQL) Exception : public core::Exception {
public:
    Exception(QSqlError const &, QString const &);

    QString errorCode() const;
    void show() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::sql
