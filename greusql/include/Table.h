#pragma once
#include <QVariant>
#include "Field.h"
#include "Index.h"
#include "greucore/include/DetailPtr.h"
#include <vector>

namespace greu::sql {

class Database;

class GREU_PUBLIC(SQL) Table {
public:
    using Record = std::vector<QVariant>;
    using Records = std::vector<Record>;
    using Fields = std::vector<Field const *>;
    struct ValuesOfField {
        Field field;
        std::vector<QVariant> values;
    };
    using ValuesOfFields = std::vector<ValuesOfField>;

    static void setReadOnly(bool = true);
    static bool isReadOnly();

    Table(QString const &name, std::initializer_list<Field> const &);
    Table(QString const &name,
          std::initializer_list<Field> const &,
          std::initializer_list<Index> const &);
    Table(QString const &name, Table const &original);
    virtual ~Table() = default;

    GREU_NO_COPY(Table);
    GREU_NO_MOVE(Table);

    QString const name() const;

protected:
    std::vector<Field> const &fields() const;
    int primaryKeyCount() const;
    Index::Indexes indexes() const;

    void create();

    Records selectAll() const;
    Records selectEqual(Record const &key, Fields const &keyFields) const;
    Records selectValues(ValuesOfFields const &keys) const;
    std::optional<Record> selectSingle(Record const &primaryKey) const;
    Records selectFromQuery(QString const &) const;

    bool recordExists(Record const &primaryKey) const;
    bool recordExists(Record const &key, Fields const &keyFields) const;

    void deleteSingle(Record const &primaryKey) const;
    void insertOrUpdate(Record const &) const;
    void update(Record const &primaryKey, Record const &values, Fields const &updateFields) const;
    void deleteAndInsert(Record const &key, Fields const &keyFields, Records const &) const;
    void deleteEqual(Record const &key, Fields const &keyFields) const;
    void deleteAll() const;

    bool isEmpty(Record const &) const;
    bool isEqualKey(Record const &, Record const &) const;
    QString toString(Record const &) const;
    QString toString(QVariant const &value, Field const &) const;

    uint64_t rowCount() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::sql
