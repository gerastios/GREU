#include "Exception.h"
#include <QMessageBox>
#include <QSqlError>

namespace greu::sql {

using namespace core;

struct Exception::Detail {
    QSqlError const error;
    QString const queryText;

    Detail(QSqlError const &e, QString const &q)
        : error(e)
        , queryText(q) {}
};

Exception::Exception(QSqlError const &e, QString const &q)
    : core::Exception(QString("%1: %2").arg(e.databaseText()).arg(q))
    , d(GREU_MKDET(e, q)) {}

QString Exception::errorCode() const {
    return d->error.nativeErrorCode();
}

void Exception::show() const {
    QMessageBox box;
    box.setWindowTitle(QObject::tr("SQL error"));
    if (d->queryText.isEmpty()) {
        box.setText(
            QObject::tr("Error code: %1\nDatabase message: %2\nDriver message:%3")
                .arg(d->error.nativeErrorCode(), d->error.databaseText(), d->error.driverText()));
    } else {
        box.setText(
            QObject::tr("Error code: %1\nDatabase message: %2\nDriver message: %3\nQuery: %3")
                .arg(d->error.nativeErrorCode())
                .arg(d->error.databaseText())
                .arg(d->error.driverText())
                .arg(d->queryText));
    }
    box.exec();
}

} // namespace greu::sql
