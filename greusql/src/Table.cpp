#include "Table.h"
#include <QDateTime>
#include <QSqlError>
#include <QString>
#include <QVariant>
#include "Exception.h"
#include "Query.h"
#include "greucore/include/Uuid.h"
#include "greucore/include/hexUtils.h"
#include <cassert>

using namespace std;

namespace {
QString const BIGINT = "bigint", BINARY = "binary", BLOB = "blob", CHAR = "char", DATE = "date",
              DATETIME = "datetime", DOUBLE = "double", FLOAT = "float", INT = "int",
              LONGBLOB = "longblob", LONGTEXT = "longtext", MEDIUMBLOB = "mediumblob",
              MEDIUMINT = "mediumint", MEDIUMTEXT = "mediumtext", SMALLINT = "smallint",
              TEXT = "text", TINYBLOB = "tinyblob", TINYINT = "tinyint", TINYTEXT = "tinytext",
              TIMESTAMP = "timestamp", TIME = "time", VARBINARY = "varbinary", VARCHAR = "varchar",
              YEAR = "year";

bool readOnly(true);
} // namespace

namespace greu::sql {

using namespace core;

struct Table::Detail {
    QString const name;
    std::vector<Field> const fields;
    Index::Indexes const indexes;
    int primaryKeyCount = 0;

    Detail(QString const &n,
           initializer_list<Field> const &f,
           initializer_list<Index> const &i = initializer_list<Index>{})
        : name(n)
        , fields(f)
        , indexes(i) {
        init();
    }

    Detail(QString const &n, Table const &o)
        : name(n)
        , fields(o.fields())
        , indexes(o.indexes()) {
        init();
    }

    void init() {
        for (auto &f : fields)
            if (f.primaryKey)
                ++primaryKeyCount;
    }

    template<typename T, typename V, typename C>
    QString concateSeparated(vector<T> fields, V v, C c) {
        QString res;
        auto notFirst = false;
        for (auto &f : fields) {
            if (!c(f))
                continue;
            if (notFirst)
                res += ", ";
            else
                notFirst = true;
            res += v(f);
        }
        return res;
    }

    void create() {
        auto qs = QString("create table if not exists ") + name + " ( "
                  + concateSeparated(
                      fields,
                      [](Field const &f) {
                          return f.name + " " + columnDefinition(f)
                                 + (f.primaryKey ? " not null " : " ");
                      },
                      [](auto) { return true; })
                  + ", primary key ( "
                  + concateSeparated(
                      fields,
                      [](Field const &f) { return f.name; },
                      [](Field const &f) { return f.primaryKey; })
                  + " )";
        for (auto &i : indexes)
            qs += QString(", ") + (i.unique ? "unique " : "") + "index " + i.name + " ( "
                  + concateSeparated(
                      i.fields, [](auto f) { return f; }, [](auto) { return true; })
                  + " ) ";
        qs += " )";
        auto q = Query::prepareQuery(qs);
        q.execute();
    }

    Records selectAll() const { return selectFromQuery("select * from " + name); }

    optional<Record> selectSingle(Record const &primaryKey) const {
        assert(int(primaryKey.size()) == primaryKeyCount);
        QString qs = "select ";
        QString qw = " where ";
        bool os = false;
        bool ow = false;
        for (Field const &f : fields) {
            if (f.primaryKey) {
                if (ow)
                    qw += " and ";
                qw += f.name + " = " + ":" + f.name;
                ow = true;
            }
            if (os)
                qs += ", ";
            qs += f.name;
            os = true;
        }
        qs += " from " + name + qw;
        auto q = Query::prepareQuery(qs);
        int i = 0;
        for (auto &f : fields)
            if (f.primaryKey)
                q.bindValue(QString(":") + f.name, primaryKey[i++]);
        q.execute();
        Record res;
        if (q.next()) {
            for (size_t i = 0; i < fields.size(); i++)
                res.push_back(q.value(i));
            return res;
        }
        return nullopt;
    }

    bool recordExists(Record const &primaryKey) const {
        assert(int(primaryKey.size()) == primaryKeyCount);
        QString qs = "select 1 from dual where exists (select * from " + name + " where ";
        bool ow = false;
        for (Field const &f : fields) {
            if (f.primaryKey) {
                if (ow)
                    qs += " and ";
                qs += f.name + " = " + ":" + f.name;
                ow = true;
            }
        }
        qs += ")";
        auto q = Query::prepareQuery(qs);
        int i = 0;
        for (auto &f : fields)
            if (f.primaryKey)
                q.bindValue(QString(":") + f.name, primaryKey[i++]);
        q.execute();
        while (q.next())
            return true;
        return false;
    }

    Records selectFromQuery(QString const &qs) const {
        auto q = Query::prepareQuery(qs);
        q.execute();
        Records res;
        while (q.next()) {
            Record r;
            for (int i = 0; i < int(fields.size()); ++i)
                r.push_back(q.value(i));
            res.push_back(r);
        }
        return res;
    }

    bool recordExists(Record const &key, Fields const &keyFields) const {
        QString qs = "select 1 from dual where exists (select * from " + name + " where ";
        bool ow = false;
        for (Field const *f : keyFields) {
            if (ow)
                qs += " and ";
            qs += f->name + " = " + ":" + f->name;
            ow = true;
        }
        qs += ")";
        auto q = Query::prepareQuery(qs);
        int i = 0;
        for (auto &f : keyFields)
            if (f->primaryKey)
                q.bindValue(QString(":") + f->name, key[i++]);
        q.execute();
        while (q.next())
            return true;
        return false;
    }

    void insertOrUpdate(Record const &record) const {
        assert(record.size() == fields.size());
        QString qc = "replace into " + name + " (";
        QString qv = " values(";
        bool oc = false;
        bool ov = false;
        for (Field const &f : fields) {
            if (oc)
                qc += ", ";
            qc += f.name;
            oc = true;
            if (ov)
                qv += ", ";
            qv += ":" + f.name;
            ov = true;
        }
        qc += ")" + qv + ")";
        auto q = Query::prepareQuery(qc);
        int i = 0;
        for (auto &f : fields)
            q.bindValue(QString(":") + f.name, record[i++]);
        q.execute();
    }

    void update(Record const &primaryKey, Record const &values, Fields const &updateFields) const {
        assert(int(primaryKey.size()) == primaryKeyCount);
        QString qs = "update " + name + " set ";
        bool os = false;
        for (auto f : updateFields) {
            if (os)
                qs += ", ";
            qs += f->name + " = " + ":" + f->name;
            os = true;
        }
        qs += " where ";
        os = false;
        for (Field const &f : fields) {
            if (f.primaryKey) {
                if (os)
                    qs += " and ";
                qs += f.name + " = " + ":" + f.name;
                os = true;
            }
        }
        auto q = Query::prepareQuery(qs);
        for (int i = 0; auto &f : fields)
            if (f.primaryKey)
                q.bindValue(QString(":") + f.name, primaryKey[i++]);
        for (int i = 0; auto f : updateFields) {
            q.bindValue(QString(":") + f->name, values[i++]);
        }
        q.execute();
    }

    void deleteSingle(Record const &primaryKey) const {
        assert(int(primaryKey.size()) == primaryKeyCount);
        QString qs = "delete from " + name + " where ";
        bool os = false;
        for (Field const &f : fields) {
            if (f.primaryKey) {
                if (os)
                    qs += " and ";
                qs += f.name + " = " + ":" + f.name;
                os = true;
            }
        }
        auto q = Query::prepareQuery(qs);
        int i = 0;
        for (auto &f : fields)
            if (f.primaryKey)
                q.bindValue(QString(":") + f.name, primaryKey[i++]);
        q.execute();
    }

    Records selectEqual(Record const &key, Fields const &keyFields) const {
        QString qs = "select * from " + name + " where ";
        bool os = false;
        for (auto f : keyFields) {
            if (os)
                qs += " and ";
            qs += f->name + " = " + ":" + f->name;
            os = true;
        }
        auto q = Query::prepareQuery(qs);
        int i = 0;
        for (auto &f : keyFields)
            q.bindValue(QString(":") + f->name, key[i++]);
        q.execute();
        return fillResultTable(q);
    }

    Records selectValues(ValuesOfFields const &keys) const {
        QString qs = "select * from " + name + " where ";
        auto ok = false;
        for (auto &k : keys) {
            if (k.values.size() > 0) {
                if (!ok)
                    qs += " and ";
                ok = true;
                if (k.values.size() == 1) {
                    qs += k.field.name + " = " + ":" + k.field.name;
                } else {
                    qs += k.field.name + " in (";
                    auto ov = false;
                    auto ik = 0;
                    for (auto &v __attribute__((unused)) : k.values) {
                        if (ov)
                            qs += ", ";
                        ov = true;
                        qs += ":" + k.field.name + QString::number(ik++);
                    }
                    qs += ")";
                }
            }
        }
        auto q = Query::prepareQuery(qs);
        for (auto &k : keys) {
            if (k.values.size() > 0) {
                if (k.values.size() == 1) {
                    q.bindValue(QString(":") + k.field.name, k.values[0]);
                } else {
                    auto ik = 0;
                    for (auto &v : k.values)
                        q.bindValue(QString(":") + k.field.name + QString::number(ik++), v);
                }
            }
        }
        q.execute();
        return fillResultTable(q);
    }

    void deleteAndInsert(Record const &key, Fields const &keyfields, Records const &records) const {
        deleteEqual(key, keyfields);
        for (auto &r : records)
            insertOrUpdate(r);
    }

    void deleteEqual(Record const &key, Fields const &keyfields) const {
        QString qs = "delete from " + name + " where ";
        bool os = false;
        for (auto f : keyfields) {
            if (os)
                qs += " and ";
            qs += f->name + " = " + ":" + f->name;
            os = true;
        }
        auto q = Query::prepareQuery(qs);
        int i = 0;
        for (auto &f : keyfields)
            q.bindValue(QString(":") + f->name, key[i++]);
        q.execute();
    }

    void deleteAll() const {
        QString qs = "delete from " + name;
        auto q = Query::prepareQuery(qs);
        q.execute();
    }

    bool isEmpty(Record const &record) const {
        int i = 0;
        for (auto &f : fields) {
            if (int(record.size()) == i)
                return true;
            switch (f.type) {
            case DataType::BIGINT:
                if (record[i].toLongLong() != 0)
                    return false;
                break;
            case DataType::INT:
            case DataType::TINYINT:
            case DataType::SMALLINT:
            case DataType::MEDIUMINT:
                if (record[i].toInt() != 0)
                    return false;
                break;
            case DataType::DOUBLE:
            case DataType::FLOAT:
                if (record[i].toDouble() != 0)
                    return false;
                break;
            case DataType::BINARY:
                if (f.length == Uuid::size())
                    if (!Uuid(record[i]).isEmpty())
                        return false;
                [[fallthrough]];
            case DataType::VARBINARY:
            case DataType::TINYBLOB:
            case DataType::BLOB:
            case DataType::MEDIUMBLOB:
            case DataType::LONGBLOB:
                if (record[i].toByteArray().size() == 0)
                    return false;
                break;
            case DataType::CHAR:
            case DataType::VARCHAR:
            case DataType::TINYTEXT:
            case DataType::TEXT:
            case DataType::MEDIUMTEXT:
            case DataType::LONGTEXT:
                if (record[i].toString().length() != 0)
                    return false;
                break;
            case DataType::DATE:
                if (!record[i].toDate().isNull())
                    return false;
                break;
            case DataType::DATETIME:
            case DataType::TIMESTAMP:
            case DataType::YEAR:
                if (!record[i].toDateTime().isNull())
                    return false;
                break;
            case DataType::TIME:
                if (!record[i].toTime().isNull())
                    return false;
                break;
                break;
            }
            i++;
        }
        return true;
    }

    bool isEqualKey(Record const &first, Record const &second) const {
        int i = 0;
        int fs = first.size();
        int ss = second.size();
        for (auto &f : fields) {
            if (fs == i || ss == i)
                return true;
            if (f.primaryKey && first[i] != second[i])
                return false;
        }
        return true;
    }

    QString toString(Record const &record) const {
        QString res;
        size_t i = 0;
        bool next = false;
        for (auto &f : fields) {
            if (i == record.size())
                return res;
            if (next)
                res += ", ";
            next = true;
            if (f.primaryKey)
                res += ">" + toString(record[i], f) + "<";
            else
                res += toString(record[i], f);
            ++i;
        }
        return res;
    }

    QString toString(QVariant const &value, Field const &field) const {
        switch (field.type) {
        case DataType::BIGINT:
            return QString::number(value.toLongLong());
        case DataType::DOUBLE:
        case DataType::FLOAT:
            return QString::number(value.toDouble());
        case DataType::BINARY:
        case DataType::VARBINARY:
        case DataType::TINYBLOB:
        case DataType::BLOB:
        case DataType::MEDIUMBLOB:
        case DataType::LONGBLOB:
            return string(value.toByteArray());
        case DataType::CHAR:
        case DataType::VARCHAR:
        case DataType::TINYTEXT:
        case DataType::TEXT:
        case DataType::MEDIUMTEXT:
        case DataType::LONGTEXT:
            return value.toString();
        case DataType::DATE:
            return value.toDate().toString();
        case DataType::DATETIME:
        case DataType::TIMESTAMP:
        case DataType::YEAR:
            return value.toDateTime().toString();
        case DataType::INT:
        case DataType::TINYINT:
        case DataType::SMALLINT:
        case DataType::MEDIUMINT:
            return QString::number(value.toInt());
        case DataType::TIME:
            return value.toTime().toString();
            break;
        }
        return QString();
    }

    void copyFrom(Table const &other, Table *that) {
        Query::prepareQuery("insert into " + that->name() + " select * from " + other.name())
            .execute();
    }

    vector<Field> readDbFields() {
        Query q;
        q.execute("show columns from " + name);
        std::vector<Field> res;
        while (q.next()) {
            auto fieldName = q.value(0).toString();
            auto fieldTypeStr = q.value(1).toString();
            auto brkt1 = fieldTypeStr.indexOf('(');
            QString fieldType;
            int fieldLength;
            if (brkt1 < 0) {
                fieldType = fieldTypeStr;
                fieldLength = 0;
            } else {
                fieldType = fieldTypeStr.left(brkt1);
                auto brkt2 = fieldTypeStr.indexOf(')');
                fieldLength = fieldTypeStr.mid(brkt1 + 1, brkt2 - brkt1 - 1).toInt();
            }
            fieldType = fieldType.toLower();
            DataType ty;
            if (fieldType == BIGINT) {
                ty = DataType::BIGINT;
                fieldLength = 0;
            } else if (fieldType == BINARY) {
                ty = DataType::BINARY;
            } else if (fieldType == CHAR) {
                ty = DataType::CHAR;
            } else if (fieldType == DATE) {
                ty = DataType::DATE;
                fieldLength = 0;
            } else if (fieldType == DATETIME) {
                ty = DataType::DATETIME;
                fieldLength = 0;
            } else if (fieldType == DOUBLE) {
                ty = DataType::DOUBLE;
                fieldLength = 0;
            } else if (fieldType == FLOAT) {
                ty = DataType::FLOAT;
                fieldLength = 0;
            } else if (fieldType == INT) {
                ty = DataType::INT;
                fieldLength = 0;
            } else if (fieldType == TINYINT) {
                ty = DataType::TINYINT;
                fieldLength = 0;
            } else if (fieldType == SMALLINT) {
                ty = DataType::SMALLINT;
                fieldLength = 0;
            } else if (fieldType == MEDIUMINT) {
                ty = DataType::MEDIUMINT;
                fieldLength = 0;
            } else if (fieldType == BIGINT) {
                ty = DataType::BIGINT;
                fieldLength = 0;
            } else if (fieldType == TIMESTAMP) {
                ty = DataType::TIMESTAMP;
            } else if (fieldType == TIME) {
                ty = DataType::TIME;
                fieldLength = 0;
            } else if (fieldType == VARBINARY) {
                ty = DataType::VARBINARY;
            } else if (fieldType == VARCHAR) {
                ty = DataType::VARCHAR;
            } else if (fieldType == YEAR) {
                ty = DataType::YEAR;
                fieldLength = 0;
            } else if (fieldType == YEAR) {
                ty = DataType::YEAR;
                fieldLength = 0;
            } else if (fieldType == YEAR) {
                ty = DataType::YEAR;
                fieldLength = 0;
            } else if (fieldType == TINYBLOB) {
                ty = DataType::TINYBLOB;
                fieldLength = 0;
            } else if (fieldType == TINYTEXT) {
                ty = DataType::TINYTEXT;
                fieldLength = 0;
            } else if (fieldType == BLOB) {
                ty = DataType::BLOB;
                fieldLength = 0;
            } else if (fieldType == TEXT) {
                ty = DataType::TEXT;
                fieldLength = 0;
            } else if (fieldType == MEDIUMBLOB) {
                ty = DataType::MEDIUMBLOB;
                fieldLength = 0;
            } else if (fieldType == MEDIUMTEXT) {
                ty = DataType::MEDIUMTEXT;
                fieldLength = 0;
            } else if (fieldType == LONGBLOB) {
                ty = DataType::LONGBLOB;
                fieldLength = 0;
            } else if (fieldType == LONGTEXT) {
                ty = DataType::LONGTEXT;
                fieldLength = 0;
            } else
                assert(false);
            res.push_back({fieldName, ty, fieldLength});
        }
        return res;
    }

    static QString columnDefinition(Field const &fld) {
        switch (fld.type) {
        case DataType::BIGINT:
            return BIGINT;
        case DataType::BINARY:
            return BINARY + "(" + QString::number(fld.length) + ")";
        case DataType::CHAR:
            return CHAR + "(" + QString::number(fld.length) + ")";
        case DataType::DATE:
            return DATE;
        case DataType::DATETIME:
            return DATETIME;
        case DataType::DOUBLE:
            return DOUBLE;
        case DataType::FLOAT:
            return FLOAT;
        case DataType::YEAR:
            return YEAR;
        case DataType::INT:
            return INT;
        case DataType::TINYINT:
            return TINYINT;
        case DataType::SMALLINT:
            return SMALLINT;
        case DataType::MEDIUMINT:
            return MEDIUMINT;
        case DataType::TIME:
            return TIME;
        case DataType::TIMESTAMP:
            return TIMESTAMP;
        case DataType::VARBINARY:
            return VARBINARY + "(" + QString::number(fld.length) + ")";
        case DataType::VARCHAR:
            return VARCHAR + "(" + QString::number(fld.length) + ")";
        case DataType::TINYBLOB:
            return TINYBLOB + "(" + QString::number(fld.length) + ")";
        case DataType::TINYTEXT:
            return TINYTEXT + "(" + QString::number(fld.length) + ")";
        case DataType::BLOB:
            return BLOB + "(" + QString::number(fld.length) + ")";
        case DataType::TEXT:
            return TEXT + "(" + QString::number(fld.length) + ")";
        case DataType::MEDIUMBLOB:
            return MEDIUMBLOB + "(" + QString::number(fld.length) + ")";
        case DataType::MEDIUMTEXT:
            return MEDIUMTEXT + "(" + QString::number(fld.length) + ")";
        case DataType::LONGBLOB:
            return LONGBLOB + "(" + QString::number(fld.length) + ")";
        case DataType::LONGTEXT:
            return LONGTEXT + "(" + QString::number(fld.length) + ")";
        }
        return QString();
    }

    Records fillResultTable(Query &q) const {
        Records res;
        while (q.next()) {
            Record r;
            for (int i = 0; i < int(fields.size()); ++i)
                r.push_back(q.value(i));
            res.push_back(r);
        }
        return res;
    }

    uint64_t rowCount() const {
        auto q = Query::prepareQuery("select ROW_COUNT()");
        q.execute();
        while (q.next()) {
            return q.value(0).toUInt();
        }
        return 0;
    }
};

void Table::setReadOnly(bool ro) {
    readOnly = ro;
}

bool Table::isReadOnly() {
    return readOnly;
}

Table::Table(QString const &n, initializer_list<Field> const &f)
    : d(GREU_MKDET(n, f)) {}

Table::Table(QString const &n, initializer_list<Field> const &f, initializer_list<Index> const &i)
    : d(GREU_MKDET(n, f, i)) {}

Table::Table(QString const &n, Table const &o)
    : d(GREU_MKDET(n, o)) {}

QString const Table::name() const {
    return d->name;
}

vector<Field> const &Table::fields() const {
    return d->fields;
}

int Table::primaryKeyCount() const {
    return d->primaryKeyCount;
}

Index::Indexes Table::indexes() const {
    return d->indexes;
}

void Table::create() {
    if (!readOnly)
        d->create();
}

Table::Records Table::selectAll() const {
    return d->selectAll();
}

Table::Records Table::selectEqual(Record const &key, Fields const &keyFields) const {
    return d->selectEqual(key, keyFields);
}

Table::Records Table::selectValues(Table::ValuesOfFields const &keys) const {
    return d->selectValues(keys);
}

optional<Table::Record> Table::selectSingle(Record const &primaryKey) const {
    return d->selectSingle(primaryKey);
}

Table::Records Table::selectFromQuery(QString const &qs) const {
    return d->selectFromQuery(qs);
}

bool Table::recordExists(Record const &primaryKey) const {
    return d->recordExists(primaryKey);
}

bool Table::recordExists(Record const &key, Fields const &keyFields) const {
    return d->recordExists(key, keyFields);
}

void Table::deleteSingle(Record const &primaryKey) const {
    d->deleteSingle(primaryKey);
}

void Table::insertOrUpdate(Record const &record) const {
    d->insertOrUpdate(record);
}

void Table::update(Record const &primaryKey,
                   Record const &values,
                   Fields const &updateFields) const {
    d->update(primaryKey, values, updateFields);
}

void Table::deleteAndInsert(Record const &key,
                            Fields const &keyFields,
                            Records const &records) const {
    d->deleteAndInsert(key, keyFields, records);
}

void Table::deleteEqual(Record const &key, Fields const &keyFields) const {
    d->deleteEqual(key, keyFields);
}

void Table::deleteAll() const {
    d->deleteAll();
}

bool Table::isEmpty(Record const &record) const {
    return d->isEmpty(record);
}

bool Table::isEqualKey(Record const &first, Record const &second) const {
    return d->isEqualKey(first, second);
}

QString Table::toString(Record const &record) const {
    return d->toString(record);
}

QString Table::toString(QVariant const &value, Field const &field) const {
    return d->toString(value, field);
}

uint64_t Table::rowCount() const {
    return d->rowCount();
}

} // namespace greu::sql
