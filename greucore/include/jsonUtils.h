#pragma once

#include <QDate>
#include <QString>
#include "greuGlobal.h"
#include <set>

class QJsonObject;

namespace greu::core {

QDate GREU_PUBLIC(CORE) valueDate(QJsonObject const &,
                                  QString const &key,
                                  QString const &format = "yyyy-MM-dd",
                                  QDate const &defaultValue = QDate());
QString GREU_PUBLIC(CORE)
    valueString(QJsonObject const &, QString const &key, QString const &defaultValue = "");
std::set<QString> GREU_PUBLIC(CORE) valuesString(QJsonObject const &, QString const &key);
std::set<QString> GREU_PUBLIC(CORE)
    valuesString(QJsonObject const &, QString const &arrayKey, QString const &idKey);
int32_t GREU_PUBLIC(CORE)
    valueInt(QJsonObject const &, QString const &key, int32_t defaultValue = 0);
uint GREU_PUBLIC(CORE) valueUIntS(QJsonObject const &, QString const &key, uint defaultValue = 0);
uint GREU_PUBLIC(CORE) valueUInt(QJsonObject const &, QString const &key, uint defaultValue = 0);
double GREU_PUBLIC(CORE)
    valueDouble(QJsonObject const &, QString const &key, double defaultValue = 0);
double GREU_PUBLIC(CORE)
    valueDoubleS(QJsonObject const &, QString const &key, double defaultValue = 0);
bool GREU_PUBLIC(CORE) valueBool(QJsonObject const &,
                                 QString const &key,
                                 QString const & = "yes",
                                 bool defaultValue = false);
std::set<int32_t> GREU_PUBLIC(CORE) valuesInt(QJsonObject const &, QString const &key);
std::set<int32_t> GREU_PUBLIC(CORE)
    valuesInt(QJsonObject const &, QString const &arrayKey, QString const &idKey);
std::set<int32_t> GREU_PUBLIC(CORE) valuesInt(QJsonValue const &, QString const &key);

} // namespace greu::core
