#include "intUtils.h"

namespace greu::core {

uint32_t fromSyncBytes32(QByteArray const &data, size_t pos) {
    assert(pos + 4 <= (size_t) data.size() && "Synchronized integer not 4 long");
    return (uchar((data.at(pos)) & 0x7F) << 21u) + (uchar((data.at(pos + 1)) & 0x7F) << 14u)
           + (uchar((data.at(pos + 2)) & 0x7F) << 7u) + (uchar(data.at(pos + 3)) & 0x7F);
}

uint32_t fromBytes32(QByteArray const &data, size_t pos) {
    assert(pos + 4 <= (size_t) data.size() && "Synchronized integer not 4 long");
    return (uchar(data.at(pos)) << 24u) + (uchar(data.at(pos + 1)) << 16u)
           + (uchar(data.at(pos + 2)) << 8u) + uchar(data.at(pos + 3));
}

QByteArray toSyncBytes32(uint32_t integer) {
    QByteArray result(4, 0x00);
    result[3] = integer % 0b10000000;
    result[2] = (integer / 0b10000000) % 0b10000000;
    result[1] = (integer / 0b0100000000000000) % 0b10000000;
    result[0] = (integer / 0b001000000000000000000000) % 0b10000000;
    return result;
}

QByteArray toBytes32(uint32_t integer) {
    QByteArray result(4, 0x00);
    result[3] = integer % 0b100000000;
    result[2] = (integer / 0b100000000) % 0b100000000;
    result[1] = (integer / 0b10000000000000000) % 0b100000000;
    result[0] = (integer / 0b1000000000000000000000000) % 0b100000000;
    return result;
}

uint32_t fromSyncBytes24(QByteArray const &data, size_t pos) {
    assert(pos + 3 <= (size_t) data.size() && "Synchronized integer not 3 long");
    return (uchar((data.at(pos)) & 0x7F) << 14u) + (uchar((data.at(pos + 1)) & 0x7F) << 7u)
           + (uchar(data.at(pos + 2)) & 0x7F);
}

uint32_t fromBytes24(QByteArray const &data, size_t pos) {
    assert(pos + 3 <= (size_t) data.size() && "Synchronized integer not 3 long");
    return (uchar(data.at(pos)) << 16u) + (uchar(data.at(pos + 1)) << 8u) + uchar(data.at(pos + 2));
}

QByteArray toSyncBytes24(uint32_t integer) {
    QByteArray result(4, 0x00);
    result[3] = integer % 0b10000000;
    result[2] = (integer / 0b10000000) % 0b10000000;
    result[1] = (integer / 0b0100000000000000) % 0b10000000;
    return result;
}

QByteArray toBytes24(uint32_t integer) {
    QByteArray result(4, 0x00);
    result[3] = integer % 0b100000000;
    result[2] = (integer / 0b100000000) % 0b100000000;
    result[1] = (integer / 0b10000000000000000) % 0b100000000;
    return result;
}

} // namespace greu::core
