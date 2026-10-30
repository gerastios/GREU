#include "Person.h"
#include <QImage>
#include "CreditHolder.h"
#include "CreditsTable.h"
#include "EmptyImages.h"
#include "Episode.h"
#include "JobsTable.h"
#include "Movie.h"
#include "PersonsTable.h"
#include "creditUtils.h"

using namespace greu::core;
using namespace greu::movies;
using namespace greu::ui;
using namespace std;

namespace greu::movies {

struct Person::Detail {
    GREU_P_DET(tmdbId, int32_t)
    GREU_P_DET(knownForJobDepartment, QString)
    GREU_P_DET(placeOfBirth, QString)
    GREU_P_DET(deathday, QDate)
    GREU_P_DET(aliases, set<QString>)

    static JobCategoriesCredits buildJobCredits(std::vector<Credit::BundledCredit> const &credits,
                                                function<QString(Credit const *)> buildJob,
                                                function<QString(Credit const *)> getJob,
                                                QString const &category = "") {
        JobCategoriesCredits res;
        using MapJobs = unordered_map<QString, JobCredit>;
        using MapSeries = unordered_map<Series *, MapJobs>;
        MapSeries map;
        for (auto const &c : credits) {
            switch (c.holder->creditHolderType()) {
            case CreditHolder::HolderType::season:
            case CreditHolder::HolderType::series:
                break;
            case CreditHolder::HolderType::episode: {
                auto episode = safeCast<Episode>(c.holder);
                auto series = episode->series();
                if (!map.contains(series))
                    map[series] = {};
                auto job = getJob(c.credit);
                auto jobString = buildJob(c.credit);
                if (!map.at(series).contains(job))
                    map.at(series)[getJob(c.credit)] = {
                        JobCredit{.job = jobString,
                                  .year = CreditHolder::dateToYear(c.holder->date()),
                                  .episodes = {episode},
                                  .series = series,
                                  .movie = nullptr}};
                else
                    map.at(series).at(job).episodes.push_back(episode);
            } break;
            case CreditHolder::HolderType::movie:
                auto dep = category.isEmpty()
                               ? JobsTable::instance()->departmentOfJob(getJob(c.credit))
                               : category;
                auto ex = ranges::find_if(res, [&dep](auto const &r) { return r.category == dep; });
                if (ex == res.end())
                    res.push_back(
                        {.category = dep,
                         .credits = {JobCredit{.job = buildJob(c.credit),
                                               .year = CreditHolder::dateToYear(c.holder->date()),
                                               .episodes = {},
                                               .series = nullptr,
                                               .movie = safeCast<Movie>(c.holder)}}});
                else
                    ex->credits.push_back(
                        JobCredit{.job = buildJob(c.credit),
                                  .year = CreditHolder::dateToYear(c.holder->date()),
                                  .episodes = {},
                                  .series = nullptr,
                                  .movie = safeCast<Movie>(c.holder)});
                break;
            }
        }
        for (auto const &s : map) {
            for (auto const &j : s.second) {
                auto dep = category.isEmpty() ? JobsTable::instance()->departmentOfJob(j.first)
                                              : category;
                auto ex = ranges::find_if(res, [&dep](auto const &r) { return r.category == dep; });
                if (ex == res.end())
                    res.push_back({.category = dep, .credits = {j.second}});
                else
                    ex->credits.push_back(j.second);
            }
        }
        for (auto &c : res) {
            ranges::sort(c.credits,
                         [](auto const &fst, auto const &snd) { return fst.year < snd.year; });
        }
        return res;
    }

    JobCategoriesCredits categorizedCredits(Person const *that) const {
        auto credits = that->credits();
        JobCategoriesCredits res;
        res.insert_range(res.begin(),
                         buildJobCredits(credits.crewCredits, buildCrewJob, getCrewJob));
        ranges::sort(res,
                     [](auto const &fst, auto const &snd) { return fst.category < snd.category; });
        res.insert_range(res.begin(),
                         buildJobCredits(credits.castCredits,
                                         buildCastJob,
                                         getCastJob,
                                         QObject::tr("Acting")));
        return res;
    }
};

Person::Person(Uuid const &u)
    : MovieObject(u)
    , d(GREU_MKDET()) {}

QImage Person::emptyImage() const {
    return EmptyImages::person();
}

Person *Person::person(core::Uuid const &u) {
    return PersonsTable::instance()->read(u);
}

int Person::age() const {
    QDate to;
    if (d->deathday.isValid())
        to = d->deathday;
    else
        to = QDate::currentDate();
    auto from = date();
    if (!from.isValid())
        return 0;
    int age = to.year() - from.year();
    // Adjust if birthday hasn't happened yet this year
    if (to.month() < from.month() || (to.month() == from.month() && to.day() < from.day())) {
        --age;
    }
    return age;
}

Person *Person::forTmdb(int32_t id) {
    return PersonsTable::instance()->readForTmdb(id);
}

void Person::read() {
    PersonsTable::instance()->read(this);
}

void Person::setTmdbId(int32_t const &v) {
    d->tmdbId = v;
}

int32_t const &Person::tmdbId() const {
    return d->tmdbId;
}

void Person::save() {
    PersonsTable::instance()->save(this);
}

void Person::update() {
    PersonsTable::instance()->readForTmdb(d->tmdbId, true);
    save();
}

GREU_P_IMP(Person, knownForJobDepartment, KnownForJobDepartment, QString)
GREU_P_IMP(Person, placeOfBirth, PlaceOfBirth, QString)
GREU_P_IMP(Person, deathday, Deathday, QDate)
GREU_P_IMP(Person, aliases, Aliases, std::set<QString>)

Credit::BundledCredits Person::credits() const {
    return CreditsTable::instance()->creditsOfPerson(uuid());
}

Person::JobCategoriesCredits Person::categorizedCredits() const {
    return d->categorizedCredits(this);
}

} // namespace greu::movies
