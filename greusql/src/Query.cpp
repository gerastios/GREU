#include "Query.h"
#include <QSqlError>
#include "Connection.h"
#include "Exception.h"

namespace greu::sql {

Query::Query()
    : QSqlQuery(*GREU_SQLDB) {}

Query::Query(QString const &q)
    : QSqlQuery("", *GREU_SQLDB) {
    prepare(q);
}

Query Query::prepareQuery(QString const &qs) {
    Query res;
    if (!res.prepare(qs))
        throw Exception(res.lastError(), res.lastQuery());
    return res;
}

void Query::prepareMe(QString const &qs) {
    if (!prepare(qs))
        throw Exception(lastError(), lastQuery());
}

void Query::execute(QString const &query) {
    if (!exec(query))
        throw Exception{lastError(), lastQuery()};
}

void Query::execute() {
    if (!exec())
        throw Exception{lastError(), lastQuery()};
}

void Query::apply(QString const &q) {
    Query qry{q};
    qry.execute();
}

} // namespace greu::sql
