#pragma once
#include <QString>
#include "boost/container_hash/hash.hpp"
#include "greuGlobal.h"
#include <vector>

namespace greu::core {

using StringPair = std::pair<QString, QString>;

enum MibNumber : int {
    Latin1 = 4,
    UTF8 = 106,
    UTF16BE = 1013,
    UTF16 = 1015,
};

bool GREU_PUBLIC(CORE) has2TerminatingZeroes(MibNumber);
QByteArray terminator(MibNumber);
QString GREU_PUBLIC(CORE) decode(QByteArray const &, MibNumber);
QByteArray GREU_PUBLIC(CORE) encode(QString const &, MibNumber);
MibNumber GREU_PUBLIC(CORE) detectMibNumber(QString const &);
std::vector<QByteArray> GREU_PUBLIC(CORE) split(QByteArray const &, MibNumber, int offset);
QString GREU_PUBLIC(CORE) getSingle(QByteArray const &, MibNumber, int offset);
std::vector<QString> GREU_PUBLIC(CORE) getMultiple(QByteArray const &, MibNumber, int offset);
// returns how similar 2 strings are:
// 1. are probably equal
// 2. Levenshtein distance of simplified strings
// 3. length of simplified 1st string
// 3. length of simplified 2nd string
std::tuple<bool, int, int, int> GREU_PUBLIC(CORE) fuzzyCompareValues(QString, QString);
bool GREU_PUBLIC(CORE) fuzzyCompare(QString const &, QString const &);
QString GREU_PUBLIC(CORE) escapeFileName(QString const &);
uint computeMillisecondsFromTimeString(QString const &);
QString GREU_PUBLIC(CORE) computeTimeStringFromMilliseconds(uint);
std::vector<QString> splitLines(QString const &);
std::vector<QString> splitLines(QByteArray const &);

} // namespace greu::core

namespace std {

template<>
struct GREU_PUBLIC(CORE) hash<greu::core::StringPair> {
    inline size_t operator()(greu::core::StringPair const &t) const {
        size_t seed = 0;
        boost::hash_combine<std::wstring>(seed, t.first.toStdWString());
        boost::hash_combine<std::wstring>(seed, t.second.toStdWString());
        return seed;
    }
};

} // namespace std
