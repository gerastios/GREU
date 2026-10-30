#include "stringUtils.h"
#include <QTextCodec>
#include <numeric>

using namespace std;

namespace {
QString const specials{" \"\t\n!§$%&/()=?\\`''*+~-_.:,;<>|{[]}#²³/"};
static uint msPerHour = 3600000;
static uint msPerMinute = 60000;
static uint msPerSecond = 1000;
static uchar constexpr zero = 0x00;
} // namespace

namespace greu::core {

int computeLevenshteinDistance(QString s1, QString s2, int l1, int l2) {
    if (l1 == 0)
        return l2;
    if (l2 == 0)
        return l1;
    s1 = s1.toLower();
    s2 = s2.toLower();
    std::vector<int> costs(l2 + 1);
    std::iota(costs.begin(), costs.end(), 0);
    int i = 0;
    for (auto &c1 : s1) {
        costs[0] = i + 1;
        auto corner = i;
        int j = 0;
        for (auto &c2 : s2) {
            auto upper = costs[j + 1];
            costs[j + 1] = (c1 == c2) ? corner : 1 + std::min(std::min(upper, corner), costs[j]);
            corner = upper;
            ++j;
        }
        ++i;
    }
    return costs[l2];
}

void simplifyString(QString &s) {
    s = s.simplified().toLower();
    for (int i = 0; i < specials.length(); i++)
        s.remove(specials.at(i));
}

bool has2TerminatingZeroes(MibNumber number) {
    switch (number) {
    case MibNumber::Latin1:
    case MibNumber::UTF8:
        return false;
    case MibNumber::UTF16:
    case MibNumber::UTF16BE:
        return true;
    }
    return false;
}

QString decode(QByteArray const &data, MibNumber number) {
    switch (number) {
    case MibNumber::Latin1:
        return QString::fromLatin1(data);
    case MibNumber::UTF8:
        return QString::fromUtf8(data);
    case MibNumber::UTF16BE: {
        QStringDecoder decoder(QStringDecoder::Utf16BE);
        QString result = decoder(data);
        return result;
    }
    case MibNumber::UTF16: {
        if (data.size() >= 2) {
            uint8_t const b0 = static_cast<quint8>(data.at(0));
            uint8_t const b1 = static_cast<quint8>(data.at(1));
            if (b0 == 0xFF && b1 == 0xFE) {
                QStringDecoder decoder(QStringDecoder::Utf16LE);
                QString result = decoder(data.mid(2));
                return result;
            }
            if (b0 == 0xFE && b1 == 0xFF) {
                QStringDecoder decoder(QStringDecoder::Utf16BE);
                QString result = decoder(data.mid(2));
                return result;
            }
        }
        // Missing BOM: little endian is by far the most common in the wild.
        QStringDecoder decoder(QStringDecoder::Utf16LE);
        return QStringDecoder{QStringDecoder::Utf16LE}(data);
    }
    }
    return QString::fromLatin1(data);
}

QByteArray encode(QString const &data, MibNumber number) {
    switch (number) {
    case MibNumber::Latin1:
        return data.toLatin1();
    case MibNumber::UTF8:
        return data.toUtf8();
    case MibNumber::UTF16BE: {
        QStringEncoder encoder(QStringEncoder::Utf16BE);
        QByteArray result = encoder(data);
        return result;
    }
    case MibNumber::UTF16: {
        QStringEncoder encoder(QStringEncoder::Utf16);
        QByteArray result = encoder(data);
        return result;
    }
    }
    return data.toUtf8();
}

MibNumber detectMibNumber(QString const &string) {
    auto bytes = string.constData();
    for (int i = 0; i < string.size(); i++)
        if (bytes[i].row() != 0x00)
            return MibNumber::UTF8;
    return MibNumber::Latin1;
}

std::vector<QByteArray> split(QByteArray const &data, MibNumber number, int offset) {
    static uchar const null = 0x00;
    static QByteArray const null2 = QByteArrayLiteral("\x00\x00");
    std::vector<QByteArray> result;
    if (has2TerminatingZeroes(number)) {
        int offset = 0;
        int len = data.length();
        while (len > 0) {
            auto f = data.mid(offset, len).indexOf(null2);
            if (f == 0) {
                offset += 2;
                len -= 2;
            } else if (f > 0) {
                result.push_back(data.mid(offset, f - 1));
                offset = offset + 1 + f;
                len = len - 1 - f;
            } else {
                result.push_back(data.mid(offset, len));
                len = 0;
            }
        }
    } else {
        if (offset > 0) {
            auto bb = data.right(data.size() - offset).split(null);
            for (auto &b : bb)
                result.push_back(b);
        } else {
            auto bb = data.split(null);
            for (auto &b : bb)
                result.push_back(b);
        }
    }
    return result;
}

QString getSingle(QByteArray const &data, MibNumber number, int offset) {
    QString result;
    auto v = split(data, number, offset);
    if (v.size() == 0)
        result = "";
    else
        result = decode(v[0], number);
    return result;
}

vector<QString> getMultiple(QByteArray const &data, MibNumber number, int offset) {
    std::vector<QString> result;
    auto parts = split(data, number, offset);
    for (auto &p : parts)
        result.push_back(decode(p, number));
    return result;
}

tuple<bool, int, int, int> fuzzyCompareValues(QString s1, QString s2) {
    simplifyString(s1);
    simplifyString(s2);
    auto l1 = s1.length();
    auto l2 = s2.length();
    auto d = computeLevenshteinDistance(s1, s2, l1, l2);
    return (d <= std::min(l1, l2) / 4) ? tuple<bool, int, int, int>{true, d, l1, l2}
                                       : tuple<bool, int, int, int>{false, d, l1, l2};
}

bool fuzzyCompare(QString const &s1, QString const &s2) {
    auto [res, d1, d2, d3] = fuzzyCompareValues(s1, s2);
    return res;
}

QByteArray terminator(MibNumber mib) {
    return (mib == MibNumber::UTF16 || mib == MibNumber::UTF16BE) ? QByteArray(2, zero)
                                                                  : QByteArray(1, zero);
}

QString escapeFileName(QString const &string) {
    auto res = string;
    res.replace("\\", "\\\\");
    res.replace(" ", "\\ ");
    res.replace("!", "\\!");
    res.replace("\"", "\\\"");
    res.replace("#", "\\#");
    res.replace("$", "\\$");
    res.replace("$", "\\&");
    res.replace("'", "\\'");
    res.replace("(", "\\(");
    res.replace(")", "\\)");
    res.replace("*", "\\*");
    res.replace(",", "\\,");
    res.replace(";", "\\;");
    res.replace("<", "\\<");
    res.replace(">", "\\>");
    res.replace("=", "\\=");
    res.replace("?", "\\?");
    res.replace("[", "\\[");
    res.replace("]", "\\]");
    res.replace("^", "\\^");
    res.replace("`", "\\`");
    res.replace("{", "\\{");
    res.replace("}", "\\}");
    res.replace("|", "\\|");
    res.replace("~", "\\~");
    return res;
}

uint computeMillisecondsFromTimeString(QString const &text) {
    uint hours = 0;
    uint minutes = 0;
    uint seconds = 0;
    uint millisecs = 0;
    auto words = text.split(":", Qt::KeepEmptyParts);
    while (words.size() < 3) {
        words.insert(words.begin(), 0);
    }
    hours = words[0].toUInt();
    minutes = words[1].toUInt();
    if (words[2].contains(".")) {
        auto ss = words[2].split(".", Qt::KeepEmptyParts);
        seconds = ss[0].toUInt();
        millisecs = ss[1].toUInt();
    } else {
        seconds = words[2].toUInt();
    }
    return hours * msPerHour + minutes * msPerMinute + seconds * msPerSecond + millisecs;
}

QString computeTimeStringFromMilliseconds(uint ms) {
    uint hours = ms / msPerHour;
    ms -= hours * msPerHour;
    uint minutes = ms / msPerMinute;
    ms -= minutes * msPerMinute;
    uint seconds = ms / msPerSecond;
    ms -= seconds * msPerSecond;
    uint millisces = ms;
    return QString{"%1:%2:%3.%4"}
        .arg(hours)
        .arg(minutes, (int) 2, (int) 10, QChar('0'))
        .arg(seconds, (int) 2, (int) 10, QChar('0'))
        .arg(millisces, (int) 3, (int) 10, QChar('0'));
}

vector<QString> splitLines(QString const &s) {
    return [s]() -> vector<QString> {
        QString sep;
        if (s.contains('\r'))
            sep = "\r\n";
        else
            sep = "\n";
        auto strings = s.split(sep, Qt::SkipEmptyParts);
        vector<QString> res;
        ranges::transform(strings, back_inserter(res), [](auto s) { return s; });
        return res;
    }();
}

vector<QString> splitLines(QByteArray const &v) {
    return splitLines(QString(v));
}

} // namespace greu::core
