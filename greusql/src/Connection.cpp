#include "Connection.h"
#include <QSqlDatabase>
#include <QSqlError>
#include "Exception.h"
#include "Query.h"

using namespace std;

namespace greu::sql {

using namespace core;

struct Connection::Detail {
    QSqlDatabase db;
    bool connected = false;
    QString schema;
};

Connection *Connection::instance() {
    static Connection i;
    return &i;
}

void Connection::connect(QString const &userName, QString const &password) {
    if (!d->db.isValid())
        d->db = QSqlDatabase::addDatabase("QMYSQL");
    d->db.setUserName(userName);
    d->db.setPassword(password);
    if (!d->db.open())
        throw Exception(d->db.lastError(), "");
    d->connected = true;
}

QSqlDatabase *Connection::database() const {
    assert(d->connected);
    return &d->db;
}

Connection::~Connection() {
    if (d->db.isOpen())
        d->db.close();
}

Connection::Connection()
    : d(GREU_MKDET()) {}

vector<QString> Connection::schemas() const {
    Query q{"show databases;"};
    q.execute();
    decltype(this->schemas()) res;
    while (q.next()) {
        auto s = q.value(0).toString();
        if (!(s == "information_schema" || s == "mysql" || s == "performance_schema" || s == "sys"))
            res.push_back(s);
    }
    return res;
}

void Connection::setSchema(QString const &s) {
    Query::apply(QString("use %1").arg(s));
    d->schema = s;
}

QString Connection::schema() const {
    return d->schema;
}

void Connection::disconnect() {
    if (d->db.isOpen())
        d->db.close();
}

} // namespace greu::sql
