#include "jsonUtils.h"
#include <QJsonArray>
#include <QJsonObject>

using namespace std;

namespace greu::core {

QString valueString(QJsonObject const &obj, QString const &key, QString const &defaultValue) {
    if (obj.contains(key)) {
        auto res = obj.value(key);
        if (!res.isString())
            return defaultValue;
        return res.toString(defaultValue);
    }
    return defaultValue;
}

int32_t valueInt(QJsonObject const &obj, QString const &key, int32_t defaultValue) {
    if (obj.contains(key)) {
        auto res = obj.value(key);
        if (!res.isDouble())
            return defaultValue;
        return res.toInteger(defaultValue);
    }
    return defaultValue;
}

uint valueUInt(QJsonObject const &obj, QString const &key, uint defaultValue) {
    if (obj.contains(key)) {
        auto res = obj.value(key);
        if (!res.isDouble())
            return defaultValue;
        return res.toInt(defaultValue);
    }
    return defaultValue;
}

uint valueUIntS(QJsonObject const &obj, QString const &key, uint defaultValue) {
    if (obj.contains(key)) {
        auto res = obj.value(key);
        if (!res.isString())
            return defaultValue;
        return res.toString("0").toUInt();
    }
    return defaultValue;
}

double valueDouble(QJsonObject const &obj, QString const &key, double defaultValue) {
    if (obj.contains(key)) {
        auto res = obj.value(key);
        if (!res.isDouble())
            return defaultValue;
        return res.toDouble(defaultValue);
    }
    return defaultValue;
}

double valueDoubleS(QJsonObject const &obj, QString const &key, double defaultValue) {
    if (obj.contains(key)) {
        auto res = obj.value(key);
        if (!res.isString())
            return defaultValue;
        return res.toString("0").toDouble();
    }
    return defaultValue;
}

bool valueBool(QJsonObject const &obj, QString const &key, QString const &trueS, bool defaultValue) {
    if (obj.contains(key)) {
        auto res = obj.value(key);
        if (!res.isString())
            return defaultValue;
        return res.toString().toLower() == trueS;
    }
    return defaultValue;
}

set<QString> valuesString(QJsonObject const &obj, QString const &key) {
    if (obj.contains(key)) {
        auto val = obj.value(key);
        if (!val.isArray())
            return {};
        set<QString> res;
        for (auto i : val.toArray())
            if (i.isString())
                res.insert(i.toString());
        return res;
    }
    return {};
}

set<QString> valuesString(QJsonObject const &obj, QString const &arrayKey, QString const &idKey) {
    if (obj.contains(arrayKey)) {
        auto val = obj.value(arrayKey);
        if (!val.isArray())
            return {};
        set<QString> res;
        for (auto i : val.toArray()) {
            if (!i.isObject())
                continue;
            auto o = i.toObject();
            if (!o.contains(idKey))
                continue;
            auto d = o.value(idKey);
            if (d.isString())
                res.insert(d.toString());
        }
        return res;
    }
    return {};
}

set<int32_t> valuesInt(QJsonObject const &obj, QString const &key) {
    if (obj.contains(key)) {
        auto val = obj.value(key);
        if (!val.isArray())
            return {};
        set<int32_t> res;
        for (auto i : val.toArray())
            if (i.isDouble())
                res.insert(i.toInteger());
        return res;
    }
    return {};
}

set<int32_t> valuesInt(QJsonObject const &obj, QString const &arrayKey, QString const &idKey) {
    if (obj.contains(arrayKey)) {
        auto val = obj.value(arrayKey);
        if (!val.isArray())
            return {};
        set<int32_t> res;
        for (auto i : val.toArray()) {
            if (!i.isObject())
                continue;
            auto o = i.toObject();
            if (!o.contains(idKey))
                continue;
            auto d = o.value(idKey);
            if (d.isDouble())
                res.insert(d.toInteger());
        }
        return res;
    }
    return {};
}

QDate valueDate(QJsonObject const &obj,
                QString const &key,
                QString const &format,
                QDate const &defaultValue) {
    if (obj.contains(key)) {
        auto res = obj.value(key);
        if (!res.isString())
            return defaultValue;
        return QDate::fromString(res.toString(), format);
    }
    return defaultValue;
}

std::set<int32_t> valuesInt(QJsonValue const &val, QString const &key) {
    if (!val.isArray())
        return {};
    set<int32_t> res;
    for (auto i : val.toArray()) {
        if (i.isDouble())
            res.insert(i.toInteger());
        else if (i.isObject())
            res.insert(i.toObject().value(key).toInteger());
    }
    return res;
}

} // namespace greu::core
