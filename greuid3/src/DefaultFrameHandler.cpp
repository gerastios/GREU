#include "DefaultFrameHandler.h"
#include <QBuffer>
#include <QImageReader>
#include <QPixmap>
#include "Frame.h"
#include "FrameHandler.h"
#include "FrameHandlerRegistry.h"
#include "ImageTypeSelector.h"
#include "Tag.h"
#include "UrlTypeSelector.h"
#include "greucore/include/Log.h"
#include "greucore/include/StaticSelector.h"
#include "greucore/include/intUtils.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/AttributeTypes.h"

using namespace std;

namespace {

using namespace greu::id3;
using namespace greu::core;

QString const DeprecartedRedordingTime{"DepTime"};

struct ParseState {
    QString year; // TYER
    QString date; // TDAT (DDMM)
    QString time; // TIME (HHMM)
};

struct SimpleHandler : public FrameHandler {
    template<typename T>
    static T *instance() {
        static T i;
        return &i;
    }
    virtual bool handles(QString const &) const override { return true; }
};

struct HandlerParseMapping {
    vector<uint> attriburteTypes;
    SimpleHandler *handler;
};

extern unordered_map<QString, HandlerParseMapping> const parseMappings;

struct HandlerRenderMapping {
    QString id;
    SimpleHandler *handler;
    uint second = 0;
};

extern unordered_map<uint, HandlerRenderMapping> const renderMappings;

uchar constexpr zero = 0x00;

using namespace greu::id3;
using namespace greu::tag;
using namespace greu::core;

MibNumber encodingToMib(uchar encoding) {
    switch ((Frame::Encoding) encoding) {
    case Frame::Encoding::Utf8:
        return greu::core::MibNumber::UTF8;
    case Frame::Encoding::Utf16:
        return greu::core::MibNumber::UTF16;
    case Frame::Encoding::Utf16BE:
        return greu::core::MibNumber::UTF16BE;
    case Frame::Encoding::Latin1:
        return greu::core::MibNumber::Latin1;
    }
    return greu::core::MibNumber::Latin1;
}

uchar mibToEncoding(MibNumber number) {
    switch (number) {
    case MibNumber::UTF8:
        return (uchar) Frame::Encoding::Utf8;
    case MibNumber::UTF16:
        return (uchar) Frame::Encoding::Utf16;
    case MibNumber::UTF16BE:
        return (uchar) Frame::Encoding::Utf16BE;
    case MibNumber::Latin1:
        return (uchar) Frame::Encoding::Latin1;
    }
    return (uchar) Frame::Encoding::Latin1;
}

QByteArray readTerminated(QByteArray const &data, size_t &pos, Frame::Encoding encoding) {
    qsizetype const start = pos;
    qsizetype const n = data.size();
    if (start >= n) {
        pos = n;
        return QByteArray();
    }

    if (encoding == Frame::Encoding::Utf16 || encoding == Frame::Encoding::Utf16BE) {
        for (qsizetype i = start; i + 1 < n; i += 2) {
            if (data.at(i) == '\0' && data.at(i + 1) == '\0') {
                pos = i + 2;
                return data.mid(start, i - start);
            }
        }
        pos = n;
        return data.mid(start);
    }

    qsizetype const term = data.indexOf('\0', start);
    if (term < 0) {
        pos = n;
        return data.mid(start);
    }
    pos = term + 1;
    return data.mid(start, term - start);
}

struct StringList {
    static vector<QString> parse(Frame const *frame) {
        vector<QString> values;
        if (frame->data().size() < 2)
            return values;
        auto const enc = frame->encoding();
        size_t pos = 1;
        while (pos < (size_t) frame->data().size()) {
            QByteArray const part = readTerminated(frame->data(), pos, enc);
            values.push_back(decode(part, encodingToMib(static_cast<uchar>(enc))));
        }
        // Drop empty values produced by (redundant) trailing terminators.
        while (!values.empty() && values.at(values.size() - 1).isEmpty())
            values.erase(values.begin() + values.size() - 1);
        return values;
    }

    static vector<QByteArray> render(AttributeTypes::ListStringAttribute const *attribute) {
        if (attribute->size() == 0)
            return {};
        QByteArray data;
        data.append(static_cast<char>(Frame::Encoding::Utf8));
        for (auto i = attribute->cbegin(); i < attribute->cend(); ++i) {
            if (i != attribute->cbegin())
                data.append(zero);
            data.append(i->toUtf8());
        }
        return {data};
    }
};

struct String {
public:
    static QString parse(Frame const *frame) {
        auto const values = StringList::parse(frame);
        if (values.size() > 0)
            return values.at(0);
        return {};
    }

    static vector<QByteArray> render(AttributeTypes::PlainStringAttribute const *attribute) {
        return {renderString(attribute->value())};
    }

    static QByteArray renderString(QString const &value) {
        if (value.isEmpty())
            return {};
        QByteArray data;
        data.append(static_cast<char>(Frame::Encoding::Utf8));
        data.append(value.toUtf8());
        return data;
    }
};

struct StringHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag, Frame const *frame, Log *, ParseData &) const override {
        safeCast<AttributeTypes::PlainStringAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)))
            ->setValue(String::parse(frame));
        return true;
    }
    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        return {{renderMappings.at(a.type()->id()).id,
                 String::render(safeCast<AttributeTypes::PlainStringAttribute const>(&a))}};
    }
};

struct DeprecatedTimeHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *, Frame const *frame, Log *, ParseData &data) const override {
        auto s = String::parse(frame);
        auto state = any_cast<ParseState>(data);
        if (frame->id() == "TYER")
            state.year = s.trimmed();
        else if (frame->id() == "TDAT")
            state.date = s.trimmed();
        else if (frame->id() == "TIME")
            state.time = s.trimmed();
        return true;
    }
    virtual FrameHandler::RenderedFrames render(Attribute const &) const override { return {}; }
    virtual void beginTag(ParseData &data) override {
        data[DeprecartedRedordingTime] = ParseState{};
    };
    virtual void endTag(ParseData &data, greu::id3::Tag *tag) override {
        if (!data.contains(DeprecartedRedordingTime))
            return;
        auto state = any_cast<ParseState>(data.at(DeprecartedRedordingTime));
        auto att = safeCast<AttributeTypes::PlainTimeAttribute>(
            tag->attribute(AttributeTypes::recordingTime()));
        if (att->isEmpty() && !state.year.isEmpty()) {
            QString timestamp = state.year;
            if (state.date.size() == 4) { // DDMM
                timestamp += QLatin1Char('-') + state.date.mid(2, 2) + QLatin1Char('-')
                             + state.date.left(2);
                if (state.time.size() == 4) // HHMM
                    timestamp += QLatin1Char(' ') + state.time.left(2) + QLatin1Char(':')
                                 + state.time.mid(2, 2);
            }
            tag->setRecordingTime(QDateTime::fromString(timestamp, "YYYY-MM-dd HH:mm"));
        }
    };
};

struct DeprecatedOrigRelYearHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag, Frame const *frame, Log *, ParseData &) const override {
        auto s = String::parse(frame);
        auto att = safeCast<AttributeTypes::PlainTimeAttribute>(
            tag->attribute(AttributeTypes::originalReleaseTime()));
        if (att->isEmpty())
            tag->originalReleaseTime().setDate(QDate::fromString(s.trimmed(), "YYYY"));
        return true;
    }
    virtual FrameHandler::RenderedFrames render(Attribute const &) const override { return {}; }
};

struct UIntPairHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag, Frame const *frame, Log *, ParseData &) const override {
        auto string = String::parse(frame);
        auto const parts = string.trimmed().split(QLatin1Char('/'));
        safeCast<AttributeTypes::PlainUintAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)))
            ->setValue(parts.value(0).trimmed().toInt());
        if (parts.size() > 1)
            safeCast<AttributeTypes::PlainUintAttribute>(
                tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(1)))
                ->setValue(parts.value(1).trimmed().toInt());
        return true;
    }
    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        auto i1 = safeCast<AttributeTypes::PlainUintAttribute const>(&a)->value();
        auto i2 = safeCast<AttributeTypes::PlainUintAttribute>(
                      a.tag()->attribute(renderMappings.at(a.type()->id()).second))
                      ->value();
        if (i1 == 0)
            return {};
        QString s = QString::number(i1);
        if (i2 > 0)
            s += QLatin1Char('/') + QString::number(i2);
        return {{renderMappings.at(a.type()->id()).id, {String::renderString(s)}}};
    }
};

struct ImageHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag,
                       Frame const *frame,
                       Log *log,
                       ParseData &) const override {
        auto frameData = frame->data();
        if (frameData.size() < 7)
            return true;
        auto mib = encodingToMib(uchar(frameData.at(0)));
        auto mime = getSingle(frameData.mid(1), MibNumber::Latin1, 0);
        auto pictureType = uchar(frameData.at(mime.length() + 2));
        auto description = getSingle(frameData.mid(mime.length() + 3), mib, 0);
        auto pictureData = frameData.mid(mime.length() + 3 + description.length()
                                         + (has2TerminatingZeroes(mib) ? 2 : 1));
        auto imgFmt = getImageFormat(mime);
        if (imgFmt.length() == 0)
            return true;
        QImage image;
        image.loadFromData(pictureData, imgFmt.toLatin1());
        auto images = tag->attributeImages();
        auto type = uint(pictureType);
        if (!images->hasKey(type)) {
            log->addWarn(QObject::tr("Invalid image type in frame %2").arg(frame->id()));
            return false;
        }
        auto rc = images->addValue(type, description, image, false);
        if (rc != Attribute::ErrorType::NoError) {
            log->addWarn(images->errorText(rc,
                                           QObject::tr("%1/%2")
                                               .arg(ImageTypeSelector::instance()->value(type),
                                                    description.isEmpty() ? QObject::tr("<empty>")
                                                                          : description)));
            return false;
        }
        return true;
    }
    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        vector<QByteArray> data;
        auto images = a.tag()->attributeImages();
        for (auto image = images->cbegin(); image != images->cend(); image++) {
            auto description = image->description;
            auto mib = detectMibNumber(description);
            QByteArray img;
            QBuffer buf(&img);
            buf.open(QIODevice::WriteOnly);
            image->value.save(&buf, image->key == ImageTypeSelector::fileIcon ? "PNG" : "JPG");
            QByteArray res;
            res.append(mibToEncoding(mib))
                .append(
                    encode(image->key == ImageTypeSelector::fileIcon ? "image/png" : "image/jpeg",
                           MibNumber::Latin1))
                .append(zero)
                .append(uchar(image->key))
                .append(encode(description, mib));
            res.append(terminator(mib));
            res.append(img);
            data.push_back(res);
        }
        return {{renderMappings.at(a.type()->id()).id, {data}}};
        ;
    }

    static QString getImageFormat(QString const &mimeType) {
        auto formats = QImageReader::imageFormatsForMimeType(mimeType.toLatin1());
        if (formats.size() == 0)
            return "";
        return formats[0];
    }
};

struct EventTimingCodesHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag,
                       Frame const *frame,
                       Log *log,
                       ParseData &) const override {
        if (frame->data().size() < 3)
            return true;
        auto values = tag->attributeEventTimingCodesValues();
        uchar uc = frame->data().at(0);
        int c = uc;
        if (!greu::id3::Tag::eventTimingCodesUnitsSelector().contains(c)) {
            log->addWarn(QObject::tr("Invalid event timing code unit: %2").arg(c));
            c = 1;
        }
        values->unit()->setValue(c);
        int ofs = 1;
        auto len = frame->data().length() - 1;
        int codeCount = 1;
        vector<uint> codes;
        while (ofs <= len) {
            if (codeCount == 0) {
                if (len - ofs >= 4) {
                    auto value = greu::core::fromBytes32(frame->data(), ofs);
                    for (auto ev : codes) {
                        auto rc = values->addValue(ev, value, false);
                        if (rc != Attribute::ErrorType::NoError)
                            log->addWarn(QObject::tr("Invalid event timing code: %1")
                                             .arg(values->errorText(rc)));
                    }
                    ofs += 4;
                    codes.clear();
                    codeCount = 1;
                } else
                    ofs = len + 1;
            } else {
                uchar uc = frame->data().at(ofs++);
                int c = uc;
                if (c == 0xff) {
                    codeCount++;
                } else {
                    if (greu::id3::Tag::eventTimingCodesTypesSelector().contains(c))
                        codes.push_back(c);
                    codeCount--;
                }
            }
        }
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        QByteArray result;
        auto values = a.tag()->attributeEventTimingCodesValues();
        if (values->size() > 0) {
            result.append(uchar(values->unit()->value()));
            for (int i = 0; i < int(values->size()); i++) {
                auto ev = values->at(i);
                result.append(uchar(ev.key)).append(greu::core::toBytes32(ev.value));
            }
            return {{renderMappings.at(a.type()->id()).id, {result}}};
            ;
        }
        return {{renderMappings.at(a.type()->id()).id, {}}};
    }
};

struct IntegerHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag, Frame const *frame, Log *, ParseData &) const override {
        safeCast<AttributeTypes::PlainUintAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)))
            ->setValue(decode(frame->data().mid(1), encodingToMib(uchar(frame->data().at(0))))
                           .trimmed()
                           .toInt());
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        auto value = safeCast<AttributeTypes::PlainUintAttribute const>(&a)->value();
        if (value == 0)
            return {{renderMappings.at(a.type()->id()).id, {}}};
        QByteArray result;
        result.append((uchar) Frame::Encoding::Latin1)
            .append(encode(QString::number(value), MibNumber::Latin1));
        return {{renderMappings.at(a.type()->id()).id, {result}}};
        ;
    }
};

struct BytesHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag, Frame const *frame, Log *, ParseData &) const override {
        if (frame->data().size() > 0)
            safeCast<AttributeTypes::PlainBytesAttribute>(
                tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)))
                ->setValue(frame->data());
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        auto value = safeCast<AttributeTypes::PlainBytesAttribute const>(&a)->value();
        if (value.length() == 0)
            return {{renderMappings.at(a.type()->id()).id, {}}};
        return {{renderMappings.at(a.type()->id()).id, {{value}}}};
        ;
    }
};

struct DateTimeHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag,
                       Frame const *frame,
                       Log *log,
                       ParseData &) const override {
        if (frame->data().size() < 5)
            return true;
        auto ts = getSingle(frame->data().mid(1), encodingToMib(uchar(frame->data().at(0))), 0);
        ts = ts.toUpper();
        auto dt = QDateTime::fromString(ts, Qt::ISODate);
        if (dt.isValid() && !dt.isNull())
            safeCast<AttributeTypes::PlainTimeAttribute>(
                tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)))
                ->setValue(dt);
        else
            log->addWarn(QObject::tr("invalid date/time in frame %1").arg(frame->id()));
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        auto value = safeCast<AttributeTypes::PlainTimeAttribute const>(&a)->value();
        if (value.isNull() || !value.isValid())
            return {{renderMappings.at(a.type()->id()).id, {}}};
        ;
        QByteArray result;
        result.append((uchar) Frame::Encoding::Latin1)
            .append(encode(value.toString(Qt::ISODate), MibNumber::Latin1));
        return {{renderMappings.at(a.type()->id()).id, {result}}};
        ;
    }
};

struct StringListHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag, Frame const *frame, Log *, ParseData &) const override {
        if (frame->data().size() < 2)
            return true;
        auto strings = getMultiple(frame->data().mid(1),
                                   encodingToMib(uchar(frame->data().at(0))),
                                   0);
        auto att = safeCast<AttributeTypes::ListStringAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)));
        for (auto s : strings) {
            att->addValue(s);
        }
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        return {{renderMappings.at(a.type()->id()).id,
                 StringList::render(safeCast<AttributeTypes::ListStringAttribute const>(&a))}};
    }
};

struct GenreHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag, Frame const *frame, Log *, ParseData &) const override {
        if (frame->data().size() < 2)
            return true;
        auto strings = getMultiple(frame->data().mid(1),
                                   encodingToMib(uchar(frame->data().at(0))),
                                   0);
        auto att = safeCast<AttributeTypes::ListStringAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)));
        for (auto s : strings) {
            auto gs = parseGenreString(s);
            for (auto g : gs)
                att->addValue(g);
        }
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        return {{renderMappings.at(a.type()->id()).id,
                 StringList::render(safeCast<AttributeTypes::ListStringAttribute const>(&a))}};
    }

    static vector<QString> parseGenreString(QString const &genreString) {
        vector<QString> genres;
        bool inBrackets = false;
        QString genre;
        for (int i = 0; i < genreString.length(); i++) {
            auto c = genreString.at(i);
            if (c == '(') {
                if (inBrackets) {
                    genre.append(c);
                } else {
                    if (genre.length() > 0) {
                        genres.push_back(genre);
                        genre = "";
                    }
                    inBrackets = true;
                }
            } else if (c == ')') {
                if (inBrackets) {
                    if (i < genreString.length() - 1 && genreString.at(i + 1) == ')') {
                        genre.append(c);
                    } else {
                        if (genre.length() > 0) {
                            genres.push_back(genre);
                            genre = "";
                        }
                        inBrackets = false;
                    }
                } else {
                    genre.append(c);
                }
            } else {
                genre.append(c);
            }
        }
        if (!genre.isEmpty())
            genres.push_back(genre);
        auto codes = greu::id3::Tag::genres();
        for (auto &g : genres) {
            if (codes.contains(g)) {
                g = codes.at(g);
            } else {
                bool ok;
                int gn = g.toInt(&ok);
                if (ok) {
                    auto gns = QString::number(gn);
                    if (codes.contains(gns))
                        g = codes.at(gns);
                }
            }
        }
        return genres;
    }
};

struct KeyedStringStringsHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag,
                       Frame const *frame,
                       Log *log,
                       ParseData &) const override {
        if (frame->data().size() < 2)
            return true;
        auto strings = getMultiple(frame->data().mid(1),
                                   encodingToMib(uchar(frame->data().at(0))),
                                   0);
        auto att = safeCast<AttributeTypes::StringKeyedListStringAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)));
        bool isKey = true;
        QString key;
        for (auto s : strings) {
            if (isKey) {
                key = s;
            } else {
                auto values = s.split(",", Qt::SkipEmptyParts);
                for (auto v : values) {
                    auto rc = att->addValue(key, v, false);
                    if (rc != Attribute::ErrorType::NoError)
                        log->addWarn(
                            QObject::tr("%1").arg(att->errorText(rc, QObject::tr("%1").arg(key))));
                }
            }
            isKey = !isKey;
        }
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        auto att = safeCast<AttributeTypes::StringKeyedListStringAttribute const>(&a);
        if (att->size() == 0)
            return {};
        QByteArray result;
        result.append(mibToEncoding(MibNumber::UTF8));
        bool first = true;
        std::vector<QString> done;
        for (auto v = att->cbegin(); v != att->cend(); v++) {
            if (!v->key.isEmpty()) {
                if (!contains(done, v->key)) {
                    done.push_back(v->key);
                    if (!first) {
                        result.append(terminator(MibNumber::UTF8));
                    }
                    first = false;
                    result.append(encode(v->key, MibNumber::UTF8));
                    result.append(zero);
                    if (has2TerminatingZeroes(MibNumber::UTF8))
                        result.append(zero);
                    bool firstPart = true;
                    for (auto vv = att->cbegin(); vv != att->cend(); vv++) {
                        if (vv->key == v->key) {
                            if (!firstPart)
                                result.append(encode(",", MibNumber::UTF8));
                            firstPart = false;
                            result.append(encode(v->value, MibNumber::UTF8));
                        }
                    }
                }
            }
        }
        return {{renderMappings.at(a.type()->id()).id, {result}}};
    }
};

struct StringKeyedListStringHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag,
                       Frame const *frame,
                       Log *log,
                       ParseData &) const override {
        if (frame->data().size() < 2)
            return true;
        auto strings = getMultiple(frame->data().mid(1),
                                   encodingToMib(uchar(frame->data().at(0))),
                                   0);
        if (strings.size() < 2)
            return true;
        auto att = safeCast<AttributeTypes::StringKeyedListStringAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)));
        auto rc = att->addValue(strings[0], strings[1], false);
        if (rc != Attribute::ErrorType::NoError)
            log->addWarn(QObject::tr("%1").arg(
                att->errorText(rc, QObject::tr("%1/%2").arg(strings[0], strings[1]))));
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        auto att = safeCast<AttributeTypes::StringKeyedListStringAttribute const>(&a);
        if (att->size() == 0)
            return {};
        vector<QByteArray> result;
        for (auto v = att->cbegin(); v != att->cend(); v++) {
            QByteArray part = QByteArray()
                                  .append(mibToEncoding(MibNumber::UTF8))
                                  .append(encode(v->key, MibNumber::UTF8))
                                  .append(zero);
            if (has2TerminatingZeroes(MibNumber::UTF8))
                part.append(zero);
            part.append(encode(v->value, MibNumber::UTF8));
            result.push_back(part);
        }
        return {{renderMappings.at(a.type()->id()).id, result}};
    }
};

struct StringKeyedListBytesHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag,
                       Frame const *frame,
                       Log *log,
                       ParseData &) const override {
        if (frame->data().size() < 3)
            return true;
        auto key = getSingle(frame->data(), MibNumber::Latin1, 0);
        auto att = safeCast<AttributeTypes::StringKeyedListBytesAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)));
        auto rc = att->addValue(key, frame->data().mid(key.length() + 1), false);
        if (rc != Attribute::ErrorType::NoError)
            log->addWarn(QObject::tr("%1").arg(att->errorText(rc, QObject::tr("%1").arg(key))));
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        auto att = safeCast<AttributeTypes::StringKeyedListBytesAttribute const>(&a);
        vector<QByteArray> result;
        for (auto v = att->cbegin(); v != att->cend(); v++) {
            result.push_back(
                QByteArray().append(encode(v->key, MibNumber::Latin1)).append(zero).append(v->value));
        }
        return {{renderMappings.at(a.type()->id()).id, result}};
    }
};

struct UserUrlHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag,
                       Frame const *frame,
                       Log *log,
                       ParseData &) const override {
        if (frame->data().size() < 2)
            return true;
        auto mib = encodingToMib(uchar(frame->data().at(0)));
        auto strings = split(frame->data().mid(1), mib, 0);
        if (strings.size() < 2)
            return true;
        auto att = safeCast<AttributeTypes::UintKeyedListStringAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)));
        auto desc = decode(strings[0], mib);
        auto rc = att->addValue(UrlTypeSelector::instance()->userValueId(),
                                desc,
                                decode(strings[1], MibNumber::Latin1),
                                false);
        if (rc != Attribute::ErrorType::NoError)
            log->addWarn(QObject::tr("%1").arg(
                att->errorText(rc,
                               QObject::tr("%1/%2")
                                   .arg(UrlTypeSelector::instance()->descriptionOfType(
                                            UrlTypeSelector::instance()->userValueId()),
                                        desc))));
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        vector<QByteArray> result;
        auto att = safeCast<AttributeTypes::UintKeyedListStringAttribute const>(&a);
        for (auto v = att->cbegin(); v != att->cend(); v++) {
            if ((int) v->key == UrlTypeSelector::instance()->userValueId()) {
                auto mib = detectMibNumber(v->description);
                QByteArray del;
                del.append(terminator(mib));
                result.push_back(QByteArray()
                                     .append(mibToEncoding(mib))
                                     .append(encode(v->description, mib))
                                     .append(del)
                                     .append(encode(v->value, MibNumber::Latin1)));
            }
        }
        return {{UrlTypeSelector::instance()->idOfType(UrlTypeSelector::instance()->userValueId()),
                 result}};
    }
};

struct UrlHandler : public SimpleHandler {
    virtual bool parse(greu::id3::Tag *tag,
                       Frame const *frame,
                       Log *log,
                       ParseData &) const override {
        if (frame->data().size() < 1)
            return true;
        auto att = safeCast<AttributeTypes::UintKeyedListStringAttribute>(
            tag->attribute(parseMappings.at(frame->id()).attriburteTypes.at(0)));
        auto ty = greu::id3::UrlTypeSelector::instance()->typeOfId(frame->id());
        auto rc = att->addValue(ty, getSingle(frame->data(), MibNumber::Latin1, 0), false);
        if (rc != Attribute::ErrorType::NoError)
            log->addWarn(QObject::tr("%1").arg(
                att->errorText(rc,
                               QObject::tr("%1").arg(
                                   UrlTypeSelector::instance()->descriptionOfType(ty)))));
        return true;
    }

    virtual FrameHandler::RenderedFrames render(Attribute const &a) const override {
        FrameHandler::RenderedFrames result;
        auto att = safeCast<AttributeTypes::UintKeyedListStringAttribute const>(&a);
        for (auto v = att->cbegin(); v != att->cend(); v++) {
            if (v->key == (uint) UrlTypeSelector::instance()->userValueId()) {
                result.append_range(UserUrlHandler::instance<UserUrlHandler>()->render(a));
            } else {
                auto id = UrlTypeSelector::instance()->idOfType(v->key);
                if (!id.isEmpty())
                    result.push_back({id, {encode(v->value, MibNumber::Latin1)}});
            }
        }
        return result;
    }
};

// clang-format off
unordered_map<QString, HandlerParseMapping> const parseMappings{
    {"APIC", {{AttributeTypes::images()}, ImageHandler::instance<ImageHandler>()}},
    {"COMM", {{AttributeTypes::comments()}, StringKeyedListStringHandler::instance<StringKeyedListStringHandler>()}},
    {"ETCO", {{AttributeTypes::eventTimingCodesValues(), AttributeTypes::eventTimingCodesUnit()}, EventTimingCodesHandler::instance<EventTimingCodesHandler>()}},
    {"MCDI", {{AttributeTypes::cdIdentifier()}, BytesHandler::instance<BytesHandler>()}},
    {"TALB", {{AttributeTypes::albumTitle()}, StringHandler::instance<StringHandler>()}},
    {"TBPM", {{AttributeTypes::beatsPerMinute()}, IntegerHandler::instance<IntegerHandler>()}},
    {"TCOM", {{AttributeTypes::composers()}, StringListHandler::instance<StringListHandler>()}},
    {"TCON", {{AttributeTypes::genres()}, GenreHandler::instance<GenreHandler>()}},
    {"TCOP", {{AttributeTypes::copyright()}, StringHandler::instance<StringHandler>()}},
    {"TDAT", {{AttributeTypes::recordingTime()}, DeprecatedTimeHandler::instance<DeprecatedTimeHandler>()}},
    {"TDEN", {{AttributeTypes::encodingTime()}, DateTimeHandler::instance<DateTimeHandler>()}},
    {"TDLY", {{AttributeTypes::playlistDelay()}, IntegerHandler::instance<IntegerHandler>()}},
    {"TDOR", {{AttributeTypes::originalReleaseTime()}, DateTimeHandler::instance<DateTimeHandler>()}},
    {"TDRC", {{AttributeTypes::recordingTime()}, DateTimeHandler::instance<DateTimeHandler>()}},
    {"TDRL", {{AttributeTypes::releaseTime()}, DateTimeHandler::instance<DateTimeHandler>()}},
    {"TDTG", {{AttributeTypes::taggingTime()}, DateTimeHandler::instance<DateTimeHandler>()}},
    {"TENC", {{AttributeTypes::encodedBy()}, StringListHandler::instance<StringListHandler>()}},
    {"TEXT", {{AttributeTypes::lyricists()}, StringListHandler::instance<StringListHandler>()}},
    {"TFLT", {{AttributeTypes::fileType()}, StringHandler::instance<StringHandler>()}},
    {"TIME", {{AttributeTypes::recordingTime()}, DeprecatedTimeHandler::instance<DeprecatedTimeHandler>()}},
    {"TIPL", {{AttributeTypes::involvedPeople()}, KeyedStringStringsHandler::instance<KeyedStringStringsHandler>()}},
    {"TIT1", {{AttributeTypes::songCategory()}, StringHandler::instance<StringHandler>()}},
    {"TIT2", {{AttributeTypes::songTitle()}, StringHandler::instance<StringHandler>()}},
    {"TIT3", {{AttributeTypes::songSubtitle()}, StringHandler::instance<StringHandler>()}},
    {"TKEY", {{AttributeTypes::key()}, StringHandler::instance<StringHandler>()}},
    {"TLAN", {{AttributeTypes::languages()}, StringListHandler::instance<StringListHandler>()}},
    {"TLEN", {{AttributeTypes::length()}, IntegerHandler::instance<IntegerHandler>()}},
    {"TMCL", {{AttributeTypes::musicians()}, KeyedStringStringsHandler::instance<KeyedStringStringsHandler>()}},
    {"TMED", {{AttributeTypes::mediaType()}, StringHandler::instance<StringHandler>()}},
    {"TMOO", {{AttributeTypes::moods()}, StringListHandler::instance<StringListHandler>()}},
    {"TOAL", {{AttributeTypes::originalAlbumTitle()}, StringHandler::instance<StringHandler>()}},
    {"TOFN", {{AttributeTypes::originalFileName()}, StringHandler::instance<StringHandler>()}},
    {"TOLY", {{AttributeTypes::originalLyricists()}, StringListHandler::instance<StringListHandler>()}},
    {"TORY", {{AttributeTypes::originalReleaseTime()}, DeprecatedOrigRelYearHandler::instance<DeprecatedOrigRelYearHandler>()}},
    {"TOPE", {{AttributeTypes::originalArtist()}, StringHandler::instance<StringHandler>()}},
    {"TOWN", {{AttributeTypes::fileOwner()}, StringHandler::instance<StringHandler>()}},
    {"TPE1", {{AttributeTypes::songArtist()}, StringHandler::instance<StringHandler>()}},
    {"TPE2", {{AttributeTypes::albumArtist()}, StringHandler::instance<StringHandler>()}},
    {"TPE3", {{AttributeTypes::conductor()}, StringHandler::instance<StringHandler>()}},
    {"TPE4", {{AttributeTypes::remixedBy()}, StringListHandler::instance<StringListHandler>()}},
    {"TPOS", {{AttributeTypes::discNumber(), AttributeTypes::discsTotal()}, UIntPairHandler::instance<UIntPairHandler>()}},
    {"TPRO", {{AttributeTypes::produced()}, StringHandler::instance<StringHandler>()}},
    {"TRCK", {{AttributeTypes::trackNumber(), AttributeTypes::tracksTotal()}, UIntPairHandler::instance<UIntPairHandler>()}},
    {"TRSN", {{AttributeTypes::radioStationName()}, StringHandler::instance<StringHandler>()}},
    {"TRSO", {{AttributeTypes::radioStationOwner()}, StringHandler::instance<StringHandler>()}},
    {"TSOA", {{AttributeTypes::albumTitleSort()}, StringHandler::instance<StringHandler>()}},
    {"TSOT", {{AttributeTypes::songTitleSort()}, StringHandler::instance<StringHandler>()}},
    {"TSOP", {{AttributeTypes::albumArtistSort()}, StringHandler::instance<StringHandler>()}},
    {"TSRC", {{AttributeTypes::isrc()}, StringHandler::instance<StringHandler>()}},
    {"TPUB", {{AttributeTypes::publisher()}, StringHandler::instance<StringHandler>()}},
    {"TSSE", {{AttributeTypes::encodingSettings()}, StringListHandler::instance<StringListHandler>()}},
    {"TSST", {{AttributeTypes::setSubtitle()}, StringHandler::instance<StringHandler>()}},
    {"TXXX", {{AttributeTypes::otherTexts()}, StringKeyedListStringHandler::instance<StringKeyedListStringHandler>()}},
    {"TYER", {{AttributeTypes::recordingTime()}, DeprecatedTimeHandler::instance<DeprecatedTimeHandler>()}},
    {"UFID", {{AttributeTypes::uniqueFileIdentifier()}, StringKeyedListBytesHandler::instance<StringKeyedListBytesHandler>()}},
    {"WCOM", {{AttributeTypes::urls()}, UrlHandler::instance<UrlHandler>()}},
    {"WCOP", {{AttributeTypes::urls()}, UrlHandler::instance<UrlHandler>()}},
    {"WOAF", {{AttributeTypes::urls()}, UrlHandler::instance<UrlHandler>()}},
    {"WOAR", {{AttributeTypes::urls()}, UrlHandler::instance<UrlHandler>()}},
    {"WOAS", {{AttributeTypes::urls()}, UrlHandler::instance<UrlHandler>()}},
    {"WORS", {{AttributeTypes::urls()}, UrlHandler::instance<UrlHandler>()}},
    {"WPAY", {{AttributeTypes::urls()}, UrlHandler::instance<UrlHandler>()}},
    {"WPUB", {{AttributeTypes::urls()}, UrlHandler::instance<UrlHandler>()}},
    {"WXXX", {{AttributeTypes::urls()}, UserUrlHandler::instance<UserUrlHandler>()}},
};
// clang-format on

// clang-format off
unordered_map<uint, HandlerRenderMapping> const renderMappings{
    {AttributeTypes::images(), {"APIC", ImageHandler::instance<ImageHandler>()}},
    {AttributeTypes::comments(), {"COMM", StringKeyedListStringHandler::instance<StringKeyedListStringHandler>()}},
    {AttributeTypes::eventTimingCodesValues(), {"ETCO", EventTimingCodesHandler::instance<EventTimingCodesHandler>()}},
    {AttributeTypes::eventTimingCodesUnit(), {"", nullptr}},
    {AttributeTypes::cdIdentifier(), {"MCDI", BytesHandler::instance<BytesHandler>()}},
    {AttributeTypes::albumTitle(), {"TALB", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::beatsPerMinute(), {"TBPM", IntegerHandler::instance<IntegerHandler>()}},
    {AttributeTypes::composers(), {"TCOM", StringListHandler::instance<StringListHandler>()}},
    {AttributeTypes::genres(), {"TCON", GenreHandler::instance<GenreHandler>()}},
    {AttributeTypes::copyright(), {"TCOP", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::encodingTime(), {"TDEN", DateTimeHandler::instance<DateTimeHandler>()}},
    {AttributeTypes::playlistDelay(), {"TDLY", IntegerHandler::instance<IntegerHandler>()}},
    {AttributeTypes::originalReleaseTime(), {"TDOR", DateTimeHandler::instance<DateTimeHandler>()}},
    {AttributeTypes::recordingTime(), {"TDRC", DateTimeHandler::instance<DateTimeHandler>()}},
    {AttributeTypes::releaseTime(), {"TDRL", DateTimeHandler::instance<DateTimeHandler>()}},
    {AttributeTypes::taggingTime(), {"TDTG", DateTimeHandler::instance<DateTimeHandler>()}},
    {AttributeTypes::encodedBy(), {"TENC", StringListHandler::instance<StringListHandler>()}},
    {AttributeTypes::lyricists(), {"TEXT", StringListHandler::instance<StringListHandler>()}},
    {AttributeTypes::fileType(), {"TFLT", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::involvedPeople(), {"TIPL", KeyedStringStringsHandler::instance<KeyedStringStringsHandler>()}},
    {AttributeTypes::songCategory(), {"TIT1", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::songTitle(), {"TIT2", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::songSubtitle(), {"TIT3", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::key(), {"TKEY", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::languages(), {"TLAN", StringListHandler::instance<StringListHandler>()}},
    {AttributeTypes::length(), {"TLEN", IntegerHandler::instance<IntegerHandler>()}},
    {AttributeTypes::musicians(), {"TMCL", KeyedStringStringsHandler::instance<KeyedStringStringsHandler>()}},
    {AttributeTypes::mediaType(), {"TMED", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::moods(), {"TMOO", StringListHandler::instance<StringListHandler>()}},
    {AttributeTypes::originalAlbumTitle(), {"TOAL", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::originalFileName(), {"TOFN", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::originalLyricists(), {"TOLY", StringListHandler::instance<StringListHandler>()}},
    {AttributeTypes::originalArtist(), {"TOPE", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::fileOwner(), {"TOWN", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::songArtist(), {"TPE1", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::albumArtist(), {"TPE2", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::conductor(), {"TPE3", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::remixedBy(), {"TPE4", StringListHandler::instance<StringListHandler>()}},
    {AttributeTypes::discNumber(), {"TPOS", UIntPairHandler::instance<UIntPairHandler>(), AttributeTypes::discsTotal()}},
    {AttributeTypes::discsTotal(), {"", nullptr}},
    {AttributeTypes::produced(), {"TPRO", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::trackNumber(), {"TRCK", UIntPairHandler::instance<UIntPairHandler>(), AttributeTypes::tracksTotal()}},
    {AttributeTypes::tracksTotal(), {"", nullptr}},
    {AttributeTypes::radioStationName(), {"TRSN", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::radioStationOwner(), {"TRSO", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::albumTitleSort(), {"TSOA", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::songTitleSort(), {"TSOT", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::albumArtistSort(), {"TSOP", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::isrc(), {"TSRC", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::publisher(), {"TPUB", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::encodingSettings(), {"TSSE", StringListHandler::instance<StringListHandler>()}},
    {AttributeTypes::setSubtitle(), {"TSST", StringHandler::instance<StringHandler>()}},
    {AttributeTypes::otherTexts(), {"TXXX", StringKeyedListStringHandler::instance<StringKeyedListStringHandler>()}},
    {AttributeTypes::uniqueFileIdentifier(), {"UFID", StringKeyedListBytesHandler::instance<StringKeyedListBytesHandler>()}},
    {AttributeTypes::urls(), {"", UrlHandler::instance<UrlHandler>()}},
};
// clang-format on

} // namespace

namespace greu::id3 {
void DefaultFrameHandler::beginTag(ParseData &data) {
    for (auto const &m : parseMappings)
        m.second.handler->beginTag(data);
}

void DefaultFrameHandler::endTag(ParseData &data, Tag *tag) {
    for (auto const &m : parseMappings)
        m.second.handler->endTag(data, tag);
}

bool DefaultFrameHandler::parse(Tag *tag, Frame const *frame, Log *log, ParseData &data) const {
    try {
        return parseMappings.at(frame->id()).handler->parse(tag, frame, log, data);
    } catch (...) {
    }
    return false;
}

FrameHandler::RenderedFrames DefaultFrameHandler::render(tag::Attribute const &a) const {
    auto const &m = renderMappings.at(a.type()->id());
    if (m.handler)
        try {
            return m.handler->render(a);
        } catch (...) {
        }
    return {};
}

} // namespace greu::id3
