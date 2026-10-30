#pragma once
#include <QSqlQuery>
#include "greucore/include/greuGlobal.h"

namespace greu::sql {

class GREU_PUBLIC(SQL) Query : public QSqlQuery {
public:
    Query();
    Query(QString const &);

    static Query prepareQuery(QString const &);

    void prepareMe(QString const &);

    GREU_MOVE(Query);

    static void apply(QString const &);

    void execute(QString const &);
    void execute();
};

} // namespace greu::sql
