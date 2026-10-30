#pragma once
#include "greucore/include/Uuid.h"

class QImage;
class QDate;

namespace greu::movies {

struct GREU_PUBLIC(MOVIES) Collectable {
    enum Type : int {
        movie = 0,
        series = 1,
        episode = 2,
        collection = 3,
        movieCollection = 4,
    };

    struct Key {
        core::Uuid uuid;
        Type type;
        inline bool operator==(Key const &o) const { return uuid == o.uuid && type == o.type; };
    };

    ~Collectable() = default;

    static QString typeString(Type);

    virtual Key key() const = 0;
    virtual QString nameInCollection() const = 0;
};

} // namespace greu::movies
