#include "Reader.h"
#include <QStringDecoder>
#include "Frame.h"
#include "FrameHandler.h"
#include "FrameHandlerRegistry.h"
#include "Tag.h"
#include "greucore/include/Log.h"
#include "greucore/include/intUtils.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/File.h"

using namespace std;

namespace {

bool isIdChar(char c) {
    return (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

bool validIdAt(QByteArray const &b, qsizetype pos, int len) {
    if (pos < 0 || pos + len > b.size())
        return false;
    for (int i = 0; i < len; ++i) {
        if (!isIdChar(b.at(pos + i)))
            return false;
    }
    return true;
}

bool isPaddingAt(QByteArray const &b, qsizetype pos, int len) {
    if (pos < 0 || pos + len > b.size())
        return false;
    for (int i = 0; i < len; ++i) {
        if (b.at(pos + i) != '\0')
            return false;
    }
    return true;
}

// ID3v2.2 (three character) -> ID3v2.4 (four character) frame identifiers.
struct IdMapping {
    char const *v22;
    char const *v24;
};

constexpr IdMapping V22Mapping[] = {
    {"BUF", "RBUF"}, {"CNT", "PCNT"}, {"COM", "COMM"}, {"CRA", "AENC"}, {"ETC", "ETCO"},
    {"GEO", "GEOB"}, {"IPL", "IPLS"}, {"LNK", "LINK"}, {"MCI", "MCDI"}, {"MLL", "MLLT"},
    {"PIC", "APIC"}, {"POP", "POPM"}, {"REV", "RVRB"}, {"SLT", "SYLT"}, {"STC", "SYTC"},
    {"TAL", "TALB"}, {"TBP", "TBPM"}, {"TCM", "TCOM"}, {"TCO", "TCON"}, {"TCP", "TCMP"},
    {"TCR", "TCOP"}, {"TDA", "TDAT"}, {"TDY", "TDLY"}, {"TEN", "TENC"}, {"TFT", "TFLT"},
    {"TIM", "TIME"}, {"TKE", "TKEY"}, {"TLA", "TLAN"}, {"TLE", "TLEN"}, {"TMT", "TMED"},
    {"TOA", "TOPE"}, {"TOF", "TOFN"}, {"TOL", "TOLY"}, {"TOR", "TORY"}, {"TOT", "TOAL"},
    {"TP1", "TPE1"}, {"TP2", "TPE2"}, {"TP3", "TPE3"}, {"TP4", "TPE4"}, {"TPA", "TPOS"},
    {"TPB", "TPUB"}, {"TRC", "TSRC"}, {"TRD", "TRDA"}, {"TRK", "TRCK"}, {"TS2", "TSO2"},
    {"TSA", "TSOA"}, {"TSC", "TSOC"}, {"TSI", "TSIZ"}, {"TSP", "TSOP"}, {"TSS", "TSSE"},
    {"TST", "TSOT"}, {"TT1", "TIT1"}, {"TT2", "TIT2"}, {"TT3", "TIT3"}, {"TXT", "TEXT"},
    {"TXX", "TXXX"}, {"TYE", "TYER"}, {"UFI", "UFID"}, {"ULT", "USLT"}, {"WAF", "WOAF"},
    {"WAR", "WOAR"}, {"WAS", "WOAS"}, {"WCM", "WCOM"}, {"WCP", "WCOP"}, {"WPB", "WPUB"},
    {"WXX", "WXXX"},
};

QString mapV22Id(QString const &id) {
    for (IdMapping const &m : V22Mapping) {
        if (id == QLatin1String(m.v22))
            return QLatin1String(m.v24);
    }
    return QString();
}

QByteArray convertPicToApic(QByteArray const &in) {
    if (in.size() < 5)
        return QByteArray();
    QByteArray const format = in.mid(1, 3).toUpper().trimmed();
    QByteArray mime;
    if (format == "JPG" || format == "JPEG")
        mime = "image/jpeg";
    else if (format == "PNG")
        mime = "image/png";
    else if (format == "GIF")
        mime = "image/gif";
    else if (format == "BMP")
        mime = "image/bmp";
    else if (format.startsWith("-->"))
        mime = "-->"; // URL reference
    else
        mime = "image/" + format.toLower();

    QByteArray out;
    out.append(in.at(0)); // text encoding
    out.append(mime);
    out.append('\0');
    out.append(in.mid(4)); // picture type, description, picture data
    return out;
}

QByteArray inflate(QByteArray const &compressed, quint32 expectedSize) {
    if (expectedSize == 0)
        expectedSize = static_cast<quint32>(compressed.size()) * 4 + 64;
    QByteArray buffer;
    buffer.append(static_cast<char>((expectedSize >> 24) & 0xFF));
    buffer.append(static_cast<char>((expectedSize >> 16) & 0xFF));
    buffer.append(static_cast<char>((expectedSize >> 8) & 0xFF));
    buffer.append(static_cast<char>(expectedSize & 0xFF));
    buffer.append(compressed);
    return qUncompress(buffer);
}

QString decodeV1String(QByteArray const &bytes) {
    bool ascii = true;
    for (char const c : bytes) {
        if (static_cast<quint8>(c) >= 0x80) {
            ascii = false;
            break;
        }
    }
    if (ascii)
        return QString::fromLatin1(bytes);

    QStringDecoder decoder(QStringDecoder::Utf8, QStringDecoder::Flag::Stateless);
    QString result = decoder(bytes);
    if (!decoder.hasError())
        return result;
    return QString::fromLatin1(bytes);
}

} // namespace

namespace greu::id3 {

using namespace core;

struct Reader::Detail {
    int major = 0;
    int revision = 0;

    void reset() {
        major = 0;
        revision = 0;
    }

    int v24FrameSize(QByteArray const &body, size_t pos, Tag &tag) {
        int const syncSafe = fromSyncBytes32(body, pos);
        int const plain = fromBytes32(body, pos);

        bool highBitSet = false;
        auto &log = tag.file()->log();
        for (qsizetype i = 4; i < 8; ++i) {
            if (static_cast<uint8_t>(body.at(pos + i)) & 0x80)
                highBitSet = true;
        }
        if (highBitSet) {
            log.addWarn(QObject::tr("Frame at offset %1 uses a non synchsafe size").arg(pos));
            return plain;
        }
        if (syncSafe == plain)
            return syncSafe;

        size_t const n = body.size();
        auto const plausibleNext = [&](uint64_t next) {
            if (next > static_cast<uint64_t>(n))
                return false;
            if (next == static_cast<uint64_t>(n))
                return true;
            size_t const p = static_cast<size_t>(next);
            return body.at(p) == '\0' || validIdAt(body, p, 4);
        };
        uint64_t const base = static_cast<uint64_t>(pos) + 10;
        if (!plausibleNext(base + syncSafe) && plausibleNext(base + plain)) {
            log.addWarn(QObject::tr("Frame at offset %1 uses a non synchsafe size").arg(pos));
            return plain;
        }
        return syncSafe;
    }

    bool parseFrame(QByteArray const &body, size_t &pos, Frame &frame, Tag &tag) {
        size_t const n = body.size();
        auto &log = tag.file()->log();
        int const idLen = (major == 2) ? 3 : 4;
        size_t const headerLen = (major == 2) ? 6 : 10;
        if (pos + headerLen > n)
            return false;

        if (!validIdAt(body, pos, idLen)) {
            log.addWarn(
                QObject::tr("Invalid frame identifier at offset %1, ignoring rest of tag").arg(pos));
            return false;
        }
        QString id = QString::fromLatin1(body.constData() + pos, idLen);

        uint32_t size = 0;
        uint8_t formatFlags = 0;
        if (major == 2) {
            size = fromBytes24(body, pos + 3);
        } else if (major == 3) {
            size = fromBytes32(body, pos + 4);
            formatFlags = static_cast<uint8_t>(body.at(pos + 9));
        } else {
            size = v24FrameSize(body, pos + 4, tag);
            formatFlags = static_cast<uint8_t>(body.at(pos + 9));
        }
        pos += headerLen;

        if (size > static_cast<uint64_t>(n - pos)) {
            log.addWarn(QObject::tr("Frame %1 exceeds the tag size, truncated").arg(id));
            size = static_cast<uint32_t>(n - pos);
        }
        QByteArray content = body.mid(pos, static_cast<size_t>(size));
        pos += static_cast<size_t>(size);

        if (major == 2) {
            QString const id4 = mapV22Id(id);
            if (id4.isEmpty()) {
                log.addWarn(QObject::tr("Frame %1 has no ID3v2.4 equivalent, dropped").arg(id));
                return true;
            }
            if (id == QLatin1String("PIC"))
                content = convertPicToApic(content);
            id = id4;
        } else if (major == 3) {
            // format flags: %(compression)(encryption)(grouping)00000
            size_t skip = 0;
            uint32_t decompressedSize = 0;
            if (formatFlags & 0x80) {
                decompressedSize = fromBytes32(content, 0);
                skip += 4;
            }
            if (formatFlags & 0x40) {
                log.addWarn(QObject::tr("Encrypted frame %1 dropped").arg(id));
                return true;
            }
            if (formatFlags & 0x20)
                skip += 1;
            if (skip > 0)
                content = content.mid(skip);
            if (formatFlags & 0x80)
                content = inflate(content, decompressedSize);
        } else {
            // format flags: %0h00kmnp  (h grouping, k compression, m encryption, n unsync, p data length)
            size_t skip = 0;
            uint32_t dataLength = 0;
            if (formatFlags & 0x40)
                skip += 1;
            if (formatFlags & 0x04) {
                log.addWarn(QObject::tr("Encrypted frame %1 dropped").arg(id));
                return true;
            }
            if (formatFlags & 0x01) {
                dataLength = fromSyncBytes32(content, skip);
                skip += 4;
            }
            if (skip > 0)
                content = content.mid(skip);
            if (formatFlags & 0x02)
                content = deunsynchronise(content);
            if (formatFlags & 0x08)
                content = inflate(content, dataLength);
        }

        if (content.isEmpty())
            return true; // empty frame -> skipped (frame stays invalid)

        frame = Frame(id, content);
        return true;
    }

    void applyFrames(vector<Frame> const &frames, Tag &tag) {
        FrameHandlerRegistry &registry = FrameHandlerRegistry::instance();

        FrameHandler::ParseData tagData;

        auto &log = tag.file()->log();

        for (auto const &addon : registry.addons())
            addon.handler->beginTag(tagData);

        for (auto const &frame : frames) {
            auto addon = registry.instanceFor(frame.id());

            bool consumed = false;
            if (addon) {
                try {
                    consumed = addon->handler->parse(&tag, &frame, &log, tagData);
                } catch (...) {
                    log.addWarn(QObject::tr("Frame handler '%1' threw while parsing %2")
                                    .arg(addon->name, frame.id()));
                }
            }
            if (!consumed)
                tag.addAdditionalFrame(frame); // nobody consumed it: written back unchanged
        }

        for (auto const &addon : registry.addons())
            addon.handler->endTag(tagData, &tag);
    }
};

Reader::Reader()
    : d(GREU_MKDET()) {}

bool Reader::readV2(QByteArray const &data, Tag &tag) {
    d->reset();
    auto &log = tag.file()->log();
    if (!isV2Header(data, 0)) {
        log.addErr(QObject::tr("No ID3v2 header found"));
        return false;
    }

    auto const major = static_cast<uint8_t>(data.at(3));
    auto const flags = static_cast<uint8_t>(data.at(5));
    d->major = major;
    d->revision = static_cast<quint8>(data.at(4));

    if (major == 2 && (flags & 0x40)) {
        log.addErr(QObject::tr("Compressed ID3v2.2 tags are not supported"));
        return false;
    }

    auto const declaredSize = fromSyncBytes32(data, 6);
    size_t const available = data.size() - V2HeaderSize;
    size_t bodySize = static_cast<size_t>(declaredSize);
    if (bodySize > available) {
        log.addWarn(
            QObject::tr("Tag is truncated (%1 of %2 bytes available)").arg(available).arg(bodySize));
        bodySize = available;
    }

    QByteArray body = data.mid(V2HeaderSize, bodySize);
    if (major < 4 && (flags & 0x80))
        body = deunsynchronise(body); // ID3v2.2/2.3: whole tag is unsynchronised

    size_t pos = 0;
    if (major >= 3 && (flags & 0x40)) { // extended header
        auto const extSize = (major == 3) ? 4 + static_cast<size_t>(fromBytes32(body, 0))
                                          : static_cast<size_t>(fromSyncBytes32(body, 0));
        if (extSize > (size_t) body.size()) {
            log.addErr(QObject::tr("Invalid extended header"));
            return false;
        }
        pos = extSize;
    }

    vector<Frame> frames;
    while (pos < (size_t) body.size()) {
        if (body.at(pos) == '\0')
            break; // padding
        Frame frame;
        if (!d->parseFrame(body, pos, frame, tag))
            break;
        if (frame.isValid())
            frames.push_back(frame);
    }

    d->applyFrames(frames, tag);
    return true;
}

bool Reader::readV1(QByteArray const &data, Tag &tag) {
    d->reset();
    auto &log = tag.file()->log();
    if (!isV1Tag(data, 0)) {
        log.addErr(QObject::tr("No ID3v1 tag found"));
        return false;
    }

    bool const v11 = data.at(125) == '\0' && data.at(126) != '\0';
    d->major = 1;
    d->revision = v11 ? 1 : 0;

    auto const field = [&data](size_t offset, size_t length) {
        QByteArray bytes = data.mid(offset, length);
        auto const zero = bytes.indexOf('\0');
        if (zero >= 0)
            bytes.truncate(zero);
        return decodeV1String(bytes).trimmed();
    };
    auto const setIfUnset = [&tag](uint attributeType, any const &value) {
        if (!tag.contains(attributeType)) {
            auto att = tag.attribute(attributeType);
            if (att->isEmpty())
                att->setGenericValue(value);
        }
    };

    setIfUnset(tag::AttributeTypes::songTitle(), field(3, 30));
    setIfUnset(tag::AttributeTypes::songArtist(), field(33, 30));
    setIfUnset(tag::AttributeTypes::albumTitle(), field(63, 30));

    QString const year = field(93, 4);
    bool numericYear = false;
    year.toInt(&numericYear);
    if (numericYear)
        setIfUnset(tag::AttributeTypes::releaseTime(),
                   QDateTime{QDate{numericYear, 1, 1}, QTime{0, 0, 0}});

    setIfUnset(tag::AttributeTypes::comments(), field(97, v11 ? 28 : 30));
    if (v11)
        setIfUnset(tag::AttributeTypes::trackNumber(),
                   static_cast<int>(static_cast<quint8>(data.at(126))));
    setIfUnset(tag::AttributeTypes::genres(),
               Tag::genreName(QString::number(static_cast<uint8_t>(data.at(127)))).trimmed());
    return true;
}

int Reader::majorVersion() const {
    return d->major;
}

int Reader::revision() const {
    return d->revision;
}

bool Reader::isV2Header(QByteArray const &data, size_t offset) {
    if (offset + V2HeaderSize > (size_t) data.size())
        return false;
    auto const *p = reinterpret_cast<uint8_t const *>(data.constData()) + offset;
    if (p[0] != 'I' || p[1] != 'D' || p[2] != '3')
        return false;

    uint8_t const major = p[3];
    uint8_t const revision = p[4];
    uint8_t const flags = p[5];
    if (major < 2 || major > 4 || revision == 0xFF)
        return false;

    uint8_t const allowedFlags = (major == 2) ? 0xC0 : ((major == 3) ? 0xE0 : 0xF0);
    if (flags & static_cast<uint8_t>(~allowedFlags))
        return false;

    for (int i = 6; i < 10; ++i) {
        if (p[i] & 0x80)
            return false;
    }
    return fromSyncBytes32(data, offset + 6) > 0;
}

qint64 Reader::v2TagSize(QByteArray const &data, size_t offset) {
    if (!isV2Header(data, offset))
        return -1;
    auto const major = static_cast<uint8_t>(data.at(offset + 3));
    auto const flags = static_cast<uint8_t>(data.at(offset + 5));
    auto total = V2HeaderSize + static_cast<int64_t>(fromSyncBytes32(data, offset + 6));
    if (major == 4 && (flags & 0x10))
        total += V2FooterSize;
    return total;
}

bool Reader::looksLikeV2Tag(QByteArray const &data, size_t offset) {
    qint64 const total = v2TagSize(data, offset);
    if (total < 0 || offset + total > (size_t) data.size())
        return false;

    auto const major = static_cast<uint8_t>(data.at(offset + 3));
    auto const flags = static_cast<uint8_t>(data.at(offset + 5));
    if (major == 2 && (flags & 0x40))
        return true; // compressed ID3v2.2 tag, frames cannot be inspected

    bool const hasFooter = (major == 4) && (flags & 0x10);
    auto const end = static_cast<size_t>(offset + total) - (hasFooter ? V2FooterSize : 0);
    size_t pos = offset + V2HeaderSize;

    if (major >= 3 && (flags & 0x40)) { // skip extended header
        pos += (major == 3) ? 4 + static_cast<size_t>(fromBytes32(data, pos))
                            : static_cast<size_t>(fromSyncBytes32(data, pos));
        if (pos > end)
            return false;
    }

    int const idLen = (major == 2) ? 3 : 4;
    if (pos + idLen > end)
        return true; // tag consists of (almost) nothing but padding
    if (isPaddingAt(data, pos, idLen))
        return true;
    return validIdAt(data, pos, idLen);
}

bool Reader::isV1Tag(QByteArray const &data, size_t offset) {
    if (offset + V1TagSize > (size_t) data.size())
        return false;
    return data.at(offset) == 'T' && data.at(offset + 1) == 'A' && data.at(offset + 2) == 'G';
}

QByteArray Reader::deunsynchronise(QByteArray const &data) {
    QByteArray out;
    out.reserve(data.size());
    qsizetype const n = data.size();
    for (qsizetype i = 0; i < n; ++i) {
        char const c = data.at(i);
        out.append(c);
        if (static_cast<quint8>(c) == 0xFF && i + 1 < n && data.at(i + 1) == '\0')
            ++i; // skip the inserted zero byte
    }
    return out;
}

} // namespace greu::id3
