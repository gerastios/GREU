#pragma once
#include <QByteArray>
#include <QString>
#include "greucore/include/DetailPtr.h"
#include <any>

namespace greu::id3 {

class GREU_PUBLIC(ID3) Frame {
public:
    enum class Encoding : uchar {
        Latin1 = 0,  ///< ISO-8859-1, terminated by 0x00
        Utf16 = 1,   ///< UTF-16 with byte order mark, terminated by 0x00 0x00
        Utf16BE = 2, ///< UTF-16 big endian without BOM (ID3v2.4 only)
        Utf8 = 3     ///< UTF-8 (ID3v2.4 only)
    };

    Frame();
    Frame(QString const &id, QByteArray const &);
    GREU_MOVE(Frame);
    GREU_COPY(Frame);

    QString id() const;
    void setId(QString const &id);

    QByteArray data() const;
    void setData(QByteArray const &data);

    bool isValid() const;

    Encoding encoding() const;

    QByteArray render() const;

    static bool isValidId(QString const &id);

    bool operator==(Frame const &other) const;
    bool operator!=(Frame const &other) const { return !(*this == other); }

    void setParseData(QString const &key, std::any);
    std::any parseData(QString const &key) const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::id3
