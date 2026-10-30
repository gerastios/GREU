#include "../include/Frame.h"
#include <QStringDecoder>
#include <QStringEncoder>
#include <any>
#include "greucore/include/intUtils.h"

using namespace std;

namespace greu::id3 {

using namespace core;

struct Frame::Detail {
    QString id;
    QByteArray data;
    unordered_map<QString, any> parseData;
};

Frame::Frame()
    : d(GREU_MKDET()) {}

Frame::Frame(QString const &id, QByteArray const &data)
    : d(GREU_MKDET()) {
    d->id = id;
    d->data = data;
}

GREU_COPY_IMPL(Frame);

QString Frame::id() const {
    return d->id;
}

void Frame::setId(QString const &id) {
    d->id = id;
}

QByteArray Frame::data() const {
    return d->data;
}

void Frame::setData(QByteArray const &data) {
    d->data = data;
}

bool Frame::isValid() const {
    return isValidId(d->id) && !d->data.isEmpty();
}

Frame::Encoding Frame::encoding() const {
    if (d->data.isEmpty())
        return Encoding::Latin1;
    uint8_t const e = static_cast<uint8_t>(d->data.at(0));
    return e <= 3 ? static_cast<Encoding>(e) : Encoding::Latin1;
}

bool Frame::isValidId(QString const &id) {
    if (id.size() != 4)
        return false;
    for (QChar const c : id) {
        char16_t const u = c.unicode();
        bool const ok = (u >= u'A' && u <= u'Z') || (u >= u'0' && u <= u'9');
        if (!ok)
            return false;
    }
    return true;
}

QByteArray Frame::render() const {
    QByteArray out;
    if (!isValid())
        return out;
    out.append(d->id.toLatin1());
    out.append(toSyncBytes32(static_cast<int>(d->data.size())));
    out.append(2, '\0'); // frame status + format flags
    out.append(d->data);
    return out;
}

bool Frame::operator==(Frame const &other) const {
    return d->id == other.d->id && d->data == other.d->data;
}

void Frame::setParseData(QString const &key, any data) {
    d->parseData[key] = data;
}

any Frame::parseData(QString const &key) const {
    try {
        return d->parseData.at(key);
    } catch (...) {
        return any{};
    }
}

} // namespace greu::id3
