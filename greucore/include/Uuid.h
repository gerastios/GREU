#pragma once

#include "DetailPtr.h"
#include "greuGlobal.h"
#include <boost/container_hash/hash.hpp>

class QVariant;
class QString;

namespace greu::core {

class GREU_PUBLIC(CORE) Uuid {
public:
    Uuid();
    explicit Uuid(QString const &);

    static Uuid emptyUuid();

    Uuid(QVariant const &value);
    Uuid(Uuid const &other);
    Uuid &operator=(Uuid const &other);

    GREU_MOVE(Uuid);

    operator QString() const;
    operator QVariant() const;
    bool isEmpty() const;

    static int size();

private:
    friend std::hash<Uuid>;
    friend bool operator==(Uuid const &, Uuid const &);
    friend bool operator!=(Uuid const &, Uuid const &);
    friend bool operator>(Uuid const &, Uuid const &);
    friend bool operator<(Uuid const &, Uuid const &);
    friend bool operator>=(Uuid const &, Uuid const &);
    friend bool operator<=(Uuid const &, Uuid const &);
    Uuid(bool);

    struct Detail;
    DetailPtr<Detail> d;
};

bool operator==(Uuid const &, Uuid const &);
bool operator!=(Uuid const &, Uuid const &);
bool operator>(Uuid const &, Uuid const &);
bool operator<(Uuid const &, Uuid const &);
bool operator>=(Uuid const &, Uuid const &);
bool operator<=(Uuid const &, Uuid const &);

using Uuids = std::vector<core::Uuid>;
using UuidSet = std::set<core::Uuid>;

} // namespace greu::core

namespace std {
template<>
struct hash<greu::core::Uuid> {
    size_t operator()(greu::core::Uuid const &u) const;
};
} // namespace std

namespace boost {
template<>
struct hash<greu::core::Uuid> {
    size_t operator()(greu::core::Uuid const &u) const;
};
} // namespace boost
