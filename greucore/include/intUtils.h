#pragma once

#include <QByteArray>
#include "greuGlobal.h"

namespace greu::core {

uint32_t GREU_PUBLIC(CORE) fromSyncBytes32(QByteArray const &, size_t);
uint32_t GREU_PUBLIC(CORE) fromBytes32(QByteArray const &, size_t);
QByteArray GREU_PUBLIC(CORE) toSyncBytes32(uint32_t);
QByteArray GREU_PUBLIC(CORE) toBytes32(uint32_t);
uint32_t GREU_PUBLIC(CORE) fromSyncBytes24(QByteArray const &, size_t);
uint32_t GREU_PUBLIC(CORE) fromBytes24(QByteArray const &, size_t);
QByteArray GREU_PUBLIC(CORE) toSyncBytes24(uint32_t);
QByteArray GREU_PUBLIC(CORE) toBytes23(uint32_t);

template<std::integral T>
bool between(T const value, T const from, T const to) {
    return value >= from && value <= to;
}

} // namespace greu::core
