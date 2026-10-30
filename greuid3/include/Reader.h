#pragma once
#include "greucore/include/DetailPtr.h"
#include <QByteArray>
#include <QString>
#include <QStringList>

namespace greu::id3 {

class Tag;

class GREU_PUBLIC(ID3) Reader
{
public:
    static constexpr size_t V2HeaderSize = 10;
    static constexpr size_t V2FooterSize = 10;
    static constexpr size_t V1TagSize = 128;

    Reader();
    GREU_NO_COPY(Reader);
    GREU_NO_MOVE(Reader);

    bool readV2(const QByteArray &data, Tag &);

    bool readV1(const QByteArray &data, Tag &);

    int majorVersion() const;
    int revision() const;

    static bool isV2Header(const QByteArray &, size_t offset = 0);

    static qint64 v2TagSize(const QByteArray &, size_t offset = 0);

    static bool looksLikeV2Tag(const QByteArray &, size_t offset = 0);

    static bool isV1Tag(const QByteArray &data, size_t offset);

    static QByteArray deunsynchronise(const QByteArray &data);

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

}
