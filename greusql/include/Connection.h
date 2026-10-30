#pragma once
#include "greucore/include/DetailPtr.h"
#include <vector>

class QSqlDatabase;

namespace greu::sql {

class GREU_PUBLIC(SQL) Connection {
public:
    static Connection *instance();
    ~Connection();
    void connect(QString const &userName, QString const &password);
    void disconnect();
    QSqlDatabase *database() const;
    std::vector<QString> schemas() const;

    void setSchema(QString const &);
    QString schema() const;

private:
    Connection();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::sql

#define GREU_SQLDB sql::Connection::instance()->database()
#define GREU_SQLTRAN sql::Connection::Transaction __tran____
