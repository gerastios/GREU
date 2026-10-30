#pragma once

#include <QObject>
#include "greuGlobal.h"
#include <boost/signals2.hpp>

class QMouseEvent;
class QFileInfo;

namespace greu::core {

using Connection = boost::signals2::connection;
using ScopedConnection = boost::signals2::scoped_connection;

using EmptySig = boost::signals2::signal<void()>;
using IntSig = boost::signals2::signal<void(int)>;
using Int64Sig = boost::signals2::signal<void(int64_t)>;
using UIntSig = boost::signals2::signal<void(uint)>;
using FloatSig = boost::signals2::signal<void(float)>;
using SizeSig = boost::signals2::signal<void(size_t)>;
using BoolSig = boost::signals2::signal<void(bool)>;
using StringSig = boost::signals2::signal<void(QString const &)>;
using DateSig = boost::signals2::signal<void(QDate const &)>;
using TimeSig = boost::signals2::signal<void(QTime const &)>;
using FileInfoSig = boost::signals2::signal<void(QFileInfo const &)>;
using DateTimeSig = boost::signals2::signal<void(QDateTime const &)>;
using PointSig = boost::signals2::signal<void(QPoint const &)>;
using ModelIndexSig = boost::signals2::signal<void(QModelIndex const &)>;
using MouseEventSig = boost::signals2::signal<void(QMouseEvent *)>;

void GREU_PUBLIC(CORE) disconnect(std::vector<ScopedConnection> &connections);
void GREU_PUBLIC(CORE) disconnect(std::vector<QMetaObject::Connection> &connections);
void GREU_PUBLIC(CORE) disconnect(QMetaObject::Connection &connection);

} // namespace greu::core
