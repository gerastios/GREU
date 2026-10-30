#pragma once
#include <QDate>
#include <QUrl>
#include "greutmdb/include/Credit.h"
#include "MovieObject.h"

namespace greu::movies {

class Credit;

struct GREU_PUBLIC(MOVIES) CreditHolder : public MovieObject {

    using MovieObject::MovieObject;

    using Credits = std::vector<Credit *>;

    enum CreditType {
        cast,
        crew,
        guestStars,
        creators,
    };

    enum HolderType {
        movie,
        series,
        season,
        episode,
    };

    virtual ~CreditHolder() = default;

    virtual Credits const *castCredits() const = 0;
    virtual Credits const *crewCredits() const = 0;
    virtual Credits const *creatorCredits() const = 0;
    virtual Credits const *guestStarsCredits() const = 0;

    virtual HolderType creditHolderType() const = 0;
    virtual std::optional<QUrl> tmdbUrl() const = 0;
    virtual inline std::vector<tmdb::Credit> tmdbCreatorCredits() const { return {}; }

    static QString dateToYear(QDate const &d) { return QString::number(d.year()); }
};

} // namespace greu::movies
