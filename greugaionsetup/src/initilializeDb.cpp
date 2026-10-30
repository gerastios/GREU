#include "initilializeDb.h"
#include <QString>
#include "greusql/include/Connection.h"
#include "greusql/include/Query.h"

using namespace greu::sql;

namespace {
void cleanup(QString const &schema) {
    Query::apply(QString("drop database if exists %1").arg(schema));
}

void create(QString const &schema) {
    Query::apply(QString("create database %1").arg(schema));
    Query::apply("create user if not exists gaion@localhost identified by 'Ga10nU53r!' password "
                 "expire never");
    Query::apply(QString("grant all on %1.* to gaion@localhost").arg(schema));
}
} // namespace

namespace greu::gaion::setup {

void initilializeDb(QString const &schema, bool resetSchema) {
    auto schemas = Connection::instance()->schemas();
    auto exists = false;
    for (auto &s : schemas) {
        if (s == schema) {
            exists = true;
            break;
        }
    }
    Connection::instance()->disconnect();
    Connection::instance()->connect("root", "My5ql123!");
    if (resetSchema || !exists) {
        cleanup(schema);
        create(schema);
    }
    Connection::instance()->disconnect();
}

} // namespace greu::gaion::setup
