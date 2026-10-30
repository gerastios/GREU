#pragma once

#include <QByteArray>
#include <QString>
#include "greuGlobal.h"

namespace greu::core {

QByteArray GREU_PUBLIC(CORE) bytes(QString const &);
QString GREU_PUBLIC(CORE) string(QByteArray const &);
QString GREU_PUBLIC(CORE) formatInput(QString const &);

template<std::integral T>
T GREU_PUBLIC(CORE) convertHexString(QString s) {
    static QString chars{"0123456789ABCDEF"};
    T res = 0;
    s = s.toUpper();
    int mul = 1;
    for (int i = s.length() - 1; i >= 0; --i) {
        res += chars.indexOf(s.at(i)) * mul;
        mul *= 16;
    }
    return res;
}

template<std::integral T>
QString GREU_PUBLIC(CORE) convertToHexString(T s) {
    static QString chars{"0123456789ABCDEF"};
    QString res = "";
    int mod = 16;
    do {
        auto pos = s % mod;
        res.prepend(chars[pos]);
        s /= mod;
        mod *= 16;
    } while (s > 0);
    return "0x" + res;
}

} // namespace greu::core
