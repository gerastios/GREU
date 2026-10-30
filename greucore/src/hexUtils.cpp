#include "hexUtils.h"

namespace greu::core {

namespace {
QString const chars{"0123456789ABCDEFabcdef"};
}

QByteArray bytes(QString const &text) {
    QByteArray result;
    if (text.simplified().isEmpty())
        return result;
    for (int i = 0; i < (text.length() - 1) / 3 + 1; i++) {
        int x = i * 3 + 1 < text.length()
                    ? chars.indexOf(text.at(i * 3)) * 16 + chars.indexOf(text.at(i * 3 + 1))
                : i * 3 < text.length() ? chars.indexOf(text.at(i * 3))
                                        : 0;
        result.append(char(x));
    }
    return result;
}

QString string(QByteArray const &bytes) {
    QString text;
    for (int i = 0; i < bytes.length(); i++) {
        auto x = int(uchar(bytes[i]));
        text.append(chars.at(x / 16));
        text.append(chars.at(x % 16));
        text.append(" ");
    }
    return text;
}

QString formatInput(QString const &text) {
    enum { first, second, blank } pos;
    pos = first;
    QString result = "";
    for (int i = 0; i < text.length(); i++) {
        auto c = text.at(i).toUpper();
        switch (pos) {
        case first:
            if (chars.indexOf(c) >= 0) {
                result.append(c);
                pos = second;
            }
            break;
        case second:
            if (chars.indexOf(c) >= 0) {
                result.append(c);
                pos = blank;
            }
            break;
        case blank:
            if (chars.indexOf(c) >= 0) {
                result.append(" ").append(c);
                pos = first;
            } else if (c == QChar::Space) {
                result.append(c);
                pos = first;
            }
            break;
        }
    }
    return result;
}

} // namespace greu::core
