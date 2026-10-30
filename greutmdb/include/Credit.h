#pragma once
#include <QString>
#include "greucore/include/greuGlobal.h"

namespace greu::tmdb {

struct GREU_PUBLIC(TMDB) Credit {
    enum Type {
        cast,
        crew,
        guestStars,
        creators,
    };
    QString id;
    Type type;
    int32_t personId;
};

struct GREU_PUBLIC(TMDB) CastCredit : public Credit {
    QString character;
    int order;
};

struct GREU_PUBLIC(TMDB) CrewCredit : public Credit {
    QString job, department;
};

} // namespace greu::tmdb
