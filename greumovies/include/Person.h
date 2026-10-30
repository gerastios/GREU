#pragma once
#include <QDate>
#include "Credit.h"
#include "MovieObject.h"
#include "property.h"

namespace greu::movies {

class Episode;
class Series;
class Movie;

class GREU_PUBLIC(MOVIES) Person : public MovieObject {
public:

    static Person *person(core::Uuid const &);

    static Person *forTmdb(int32_t);

    virtual void read() override;
    virtual void save() override;
    virtual void update() override;

    GREU_P_DEF(tmdbId, TmdbId, int32_t)
    GREU_P_DEF(knownForJobDepartment, KnownForJobDepartment, QString)
    GREU_P_DEF(placeOfBirth, PlaceOfBirth, QString)
    GREU_P_DEF(deathday, Deathday, QDate)
    GREU_P_DEF(aliases, Aliases, std::set<QString>)

    int age() const;

    virtual ObjectType objectType() const override { return ObjectType::person; }

    Credit::BundledCredits credits() const;

    struct JobCredit {
        QString job;
        QString year;
        std::vector<Episode *> episodes;
        Series *series;
        Movie *movie;
    };

    struct JobCategoryCredits {
        QString category;
        std::vector<JobCredit> credits;
    };
    using JobCategoriesCredits = std::vector<JobCategoryCredits>;

    JobCategoriesCredits categorizedCredits() const;

private:
    friend class Object;
    Person(core::Uuid const &);
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual QImage emptyImage() const override;
};

} // namespace greu::movies
