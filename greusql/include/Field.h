#pragma once
#include <QString>
#include <QVariant>
#include "greucore/include/greuGlobal.h"

namespace greu::sql {

enum class GREU_PUBLIC(SQL) DataType {
    TINYINT,   // 8
    SMALLINT,  // 16
    MEDIUMINT, // 24
    INT,       // 32
    BIGINT,    // 64
    DATE,
    DATETIME,
    DOUBLE,
    FLOAT,
    TIMESTAMP,
    TIME,
    YEAR,
    BINARY,     // < 255
    VARBINARY,  // < 255
    CHAR,       // < 255
    VARCHAR,    // < 255
    TINYBLOB,   // < 256 B
    TINYTEXT,   // < 256 B
    BLOB,       // < 64 KB
    TEXT,       // < 64 KB
    MEDIUMBLOB, // < 16 MB
    MEDIUMTEXT, // < 16 MB
    LONGBLOB,   // < 4 GB
    LONGTEXT,   // < 4 GB
};

struct GREU_PUBLIC(SQL) Field {
    using IntRange = std::pair<int, int>;

    enum CaseConversion { Lower, Upper, None };

    inline Field(QString const &name, DataType type, bool primaryKey = false)
        : name(name)
        , type(type)
        , length(0)
        , primaryKey(primaryKey)
        , caption("")
        , caseConversion(CaseConversion::None)
        , intRange({0, 0}) {}

    inline Field(QString const &name, DataType type, int length, bool primaryKey = false)
        : name(name)
        , type(type)
        , length(length)
        , primaryKey(primaryKey)
        , caption("")
        , caseConversion(CaseConversion::None)
        , intRange({0, 0}) {}

    inline Field(QString const &name, DataType type, QString const &caption, bool primaryKey = false)
        : name(name)
        , type(type)
        , length(0)
        , primaryKey(primaryKey)
        , caption(caption)
        , caseConversion(CaseConversion::None)
        , intRange({0, 0}) {}

    inline Field(QString const &name,
                 DataType type,
                 int length,
                 QString const &caption,
                 bool primaryKey = false)
        : name(name)
        , type(type)
        , length(length)
        , primaryKey(primaryKey)
        , caption(caption)
        , caseConversion(CaseConversion::None)
        , intRange({0, 0}) {}

    inline Field(QString const &name,
                 DataType type,
                 QString const &caption,
                 CaseConversion caseConversion,
                 bool primaryKey = false)
        : name(name)
        , type(type)
        , length(0)
        , primaryKey(primaryKey)
        , caption(caption)
        , caseConversion(caseConversion)
        , intRange({0, 0}) {}

    inline Field(QString const &name,
                 DataType type,
                 int length,
                 QString const &caption,
                 CaseConversion caseConversion,
                 bool primaryKey = false)
        : name(name)
        , type(type)
        , length(length)
        , primaryKey(primaryKey)
        , caption(caption)
        , caseConversion(caseConversion)
        , intRange({0, 0}) {}

    inline Field(QString const &name,
                 DataType type,
                 QString const &caption,
                 IntRange const &intRange,
                 bool primaryKey = false)
        : name(name)
        , type(type)
        , length(0)
        , primaryKey(primaryKey)
        , caption(caption)
        , caseConversion(CaseConversion::None)
        , intRange(intRange) {}

    inline Field(QString const &name,
                 DataType type,
                 int length,
                 QString const &caption,
                 IntRange const &intRange,
                 bool primaryKey = false)
        : name(name)
        , type(type)
        , length(length)
        , primaryKey(primaryKey)
        , caption(caption)
        , caseConversion(CaseConversion::None)
        , intRange(intRange) {}

    QString const name;
    DataType const type;
    int const length;
    bool const primaryKey;
    QString const caption;
    CaseConversion const caseConversion;
    IntRange const intRange;
};

} // namespace greu::sql
