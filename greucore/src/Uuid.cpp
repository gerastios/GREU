#include "Uuid.h"
#include <QVariant>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_hash.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <cassert>

using namespace std;
using namespace boost::uuids;

namespace greu::core {

struct Uuid::Detail {
    uuid uuid_;
    static Uuid ini;
    string_generator stringParser;
};

Uuid Uuid::Detail::ini(true);

Uuid::Uuid()
    : d(GREU_MKDET()) {
    d->uuid_ = random_generator()();
}

Uuid::Uuid(QString const &s)
    : d(GREU_MKDET()) {
    d->uuid_ = d->stringParser(s.toStdString());
}

Uuid Uuid::emptyUuid() {
    return Detail::ini;
}

Uuid::Uuid(bool)
    : d(GREU_MKDET()) {}

Uuid::Uuid(QVariant const &value)
    : d(GREU_MKDET()) {
    auto b = value.toByteArray();
    assert(b.size() == (qsizetype) d->uuid_.size() && "Wrong UUID size");
    memcpy(d->uuid_.data, b.data(), d->uuid_.size());
}

Uuid::Uuid(Uuid const &other)
    : d(GREU_MKDET()) {
    d->uuid_ = other.d->uuid_;
}

int Uuid::size() {
    return uuid::static_size();
}

Uuid &Uuid::operator=(Uuid const &other) {
    d->uuid_ = other.d->uuid_;
    return *this;
}

Uuid::operator QString() const {
    if (isEmpty())
        return "";
    return QString::fromStdString(to_string(d->uuid_));
}

Uuid::operator QVariant() const {
    QByteArray b(d->uuid_.size(), 0);
    memcpy(b.data(), d->uuid_.data, d->uuid_.size());
    return QVariant(b);
}

bool Uuid::isEmpty() const {
    return *this == Detail::ini;
}

bool operator==(Uuid const &l, Uuid const &r) {
    return l.d->uuid_ == r.d->uuid_;
}

bool operator!=(Uuid const &l, Uuid const &r) {
    return l.d->uuid_ != r.d->uuid_;
}

bool operator>(Uuid const &l, Uuid const &r) {
    return l.d->uuid_ > r.d->uuid_;
}

bool operator<(Uuid const &l, Uuid const &r) {
    return l.d->uuid_ < r.d->uuid_;
}

bool operator>=(Uuid const &l, Uuid const &r) {
    return l.d->uuid_ >= r.d->uuid_;
}

bool operator<=(Uuid const &l, Uuid const &r) {
    return l.d->uuid_ <= r.d->uuid_;
}

} // namespace greu::core

using namespace greu::core;

size_t hash<Uuid>::operator()(Uuid const &u) const {
    return boost::hash<uuid>{}(u.d->uuid_);
}
