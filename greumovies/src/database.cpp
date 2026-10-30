#include "database.h"
#include "FilesTable.h"
#include "greusql/include/Connection.h"
#include "greusql/include/Query.h"

using namespace std;
using namespace greu::core;
using namespace greu::sql;

namespace {
QString const user{"gaion"};
QString const password{"Ga10nU53r!"};
QString const setupUser{"gaionsetup"};
QString const setupPassword{"Ga10n53tupU53r!"};
} // namespace

namespace greu::movies {

void openDatabase(QString const& db, bool setup) {
    if (setup)
        Connection::instance()->connect(setupUser, setupPassword);
    else
        Connection::instance()->connect(user, password);
    Connection::instance()->setSchema(db);
}

void initDatabase(QString const& db, QString const& rootPass) {
    Connection::instance()->connect("root", rootPass);
    Query::apply(QString("drop database if exists %1").arg(db));
    Query::apply(QString("create database %1").arg(db));
    Query::apply(QString("use %1").arg(db));
    Table::setReadOnly(false);
    FilesTable::instance();
    Query::apply(QString("create user if not exists %1@localhost identified by '%2' password "
                         "expire never")
                     .arg(user)
                     .arg(password));
    Query::apply(QString("grant select on %1.* to %2@localhost").arg(db).arg(user));
    Query::apply(QString("grant update on %1.files to %2@localhost").arg(db).arg(user));
    Query::apply(QString("create user if not exists %1@localhost identified by '%2' password "
                         "expire never")
                     .arg(setupUser)
                     .arg(setupPassword));
    Query::apply(QString("grant all on %1.* to %2@localhost").arg(db).arg(setupUser));
    Connection::instance()->disconnect();
}

} // namespace greu::movies
