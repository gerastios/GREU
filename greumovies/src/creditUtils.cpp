#include "creditUtils.h"
#include "Credit.h"
#include "JobsTable.h"
#include "Person.h"

using namespace std;
using namespace greu::movies;
using namespace greu::core;

namespace {

MediumDepartmentsCredits buildDepartments(auto credits,
                                          function<QString(Credit const *)> buildJob,
                                          function<QString(Credit const *)> getJob,
                                          QString const &category = "") {
    unordered_map<QString, CreditHolder::Credits> mapped;
    for (auto const &c : credits) {
        auto dep = category.isEmpty() ? JobsTable::instance()->departmentOfJob(getJob(c))
                                      : category;
        if (!mapped.contains(dep))
            mapped[dep] = {c};
        else
            mapped.at(dep).push_back(c);
    }
    MediumDepartmentsCredits res;
    ranges::transform(mapped,
                      back_inserter(res),
                      [&buildJob](pair<QString, CreditHolder::Credits> const &p) {
                          vector<MediumCredit> cr;
                          ranges::transform(p.second, back_inserter(cr), [&buildJob](Credit *c) {
                              return MediumCredit{.job = buildJob(c),
                                                  .person = Person::person(c->personId())};
                          });
                          return MediumDepartmentCredits{p.first, cr};
                      });
    ranges::sort(res,
                 [](auto const &fst, auto const &snd) { return fst.department < snd.department; });
    return res;
}

template<class LESS>
using sortedStrings = set<QString, LESS>;

template<class LESS>
struct Prio {
    uint priority;
    sortedStrings<LESS> values;
};

template<class LESS>
struct Priority {
    Uuid id;
    sortedStrings<LESS> values;
};

struct ActorInfo {
    uint order;
    Uuid id;
    QString character;
};

template<class LESS>
using NamedPrios = unordered_map<Uuid, Prio<LESS>>;
template<class LESS>
using SortedPriorities = multimap<uint, Priority<LESS>, greater<uint>>;

template<class T>
void addPrio(NamedPrios<T> &prios, uint prio, Uuid id, QString const &value) {
    if (!prios.contains(id)) {
        prios.insert({id, {prio, {value}}});
        return;
    }
    auto &p = prios.at(id);
    p.priority += prio;
    p.values.insert(value);
}

uint jobPriority(QString const &j) {
    if (j == "Director")
        return 5;
    else if (j == "Screenplay")
        return 4;
    else if (j == "Writer")
        return 4;
    else if (j == "Book")
        return 4;
    else if (j == "Novel")
        return 4;
    else if (j == "Producer")
        return 3;
    else if (j == "Visual Effect")
        return 1;
    return 0;
}

struct JobLess {
    bool operator()(QString const &fst, QString const &snd) const {
        return jobPriority(fst) < jobPriority(snd);
    }
};

MediumCredits simpleGetter(int max, std::function<CreditHolder::Credits const *()> f) {
    auto credits = f();
    if (!credits)
        return {};
    MediumCredits res;
    ranges::transform(*credits, back_inserter(res), [](auto &c) {
        return MediumCredit{c->job().isEmpty() ? c->character() : c->job(),
                            Person::person(c->personId())};
    });
    if (max == 0)
        return res;
    return ranges::to<MediumCredits>(res | views::take(std::min((size_t) max, res.size())));
}

} // namespace

namespace greu::movies {

QString buildCrewJob(Credit const *credit) {
    return QString("... %1").arg(credit->job());
}
QString getCrewJob(Credit const *credit) {
    return credit->job();
}
QString buildCastJob(Credit const *credit) {
    return QString("as %1").arg(credit->character());
}
QString getCastJob(Credit const *credit) {
    return credit->character();
}

MediumDepartmentsCredits completeCast(CreditHolder const *holder) {
    auto cast = views::concat(*holder->castCredits(), *holder->guestStarsCredits());
    return buildDepartments(cast, buildCastJob, getCastJob);
}

MediumDepartmentsCredits completeCrew(CreditHolder const *holder) {
    auto crew = views::concat(*holder->crewCredits(), *holder->creatorCredits());
    return buildDepartments(crew, buildCrewJob, getCrewJob);
}

MediumDepartmentsCredits complete(CreditHolder const *holder) {
    return ranges::to<MediumDepartmentsCredits>(
        views::concat(completeCast(holder), completeCrew(holder)));
}

MediumCredits importantCrew(CreditHolder const *holder, int max) {
    return importantCrew(vector{holder}, max);
}

MediumCredits importantCast(CreditHolder const *holder, int max) {
    return importantCast(vector{holder}, max);
}

MediumCredits importantCrew(vector<CreditHolder const *> const &holders, int max) {
    NamedPrios<JobLess> prios;
    for (auto const &holder : holders) {
        auto crewCredits = holder->crewCredits();
        if (!crewCredits)
            continue;
        for (auto &c : *crewCredits) {
            auto job = c->job();
            auto p = jobPriority(job);
            if (p > 0)
                addPrio(prios, p, c->personId(), job);
        }
    }
    SortedPriorities<JobLess> sp;
    for (auto &p : prios)
        sp.insert({p.second.priority, {p.first, p.second.values}});
    MediumCredits res;
    for (int i = 0; auto &p : sp) {
        res.push_back(
            MediumCredit{ranges::to<QString>(p.second.values | views::join_with(QString(", "))),
                         Person::person(p.second.id)});
        if (++i == max && max > 0)
            break;
    }
    return res;
}

MediumCredits importantCast(vector<CreditHolder const *> const &holders, int max) {
    vector<ActorInfo> cast;
    for (auto const &holder : holders) {
        auto castCredits = holder->castCredits();
        if (!castCredits)
            continue;
        for (auto &c : *castCredits) {
            auto exist = ranges::find_if(cast, [&c](ActorInfo const &e) {
                return e.character == c->character() && e.id == c->personId();
            });
            if (exist == cast.end())
                cast.push_back({c->order(), c->personId(), c->character()});
            else if (exist->order < c->order())
                exist->order = c->order();
        }
    }
    ranges::sort(cast, [](ActorInfo const &f, ActorInfo const &s) { return f.order < s.order; });
    MediumCredits res;
    ranges::transform(cast, back_inserter(res), [](auto &c) {
        return MediumCredit{c.character, Person::person(c.id)};
    });
    if (max == 0)
        return res;
    return ranges::to<MediumCredits>(res | views::take(std::min((size_t) max, res.size())));
}

MediumCredits creators(CreditHolder const *holder, int max) {
    return simpleGetter(max, [holder] { return holder->creatorCredits(); });
}

MediumCredits guestStars(CreditHolder const *holder, int max) {
    return simpleGetter(max, [holder] { return holder->guestStarsCredits(); });
}

MediumCredits crewForJob(CreditHolder const *holder, QString const &job, int max) {
    CreditHolder::Credits credits;
    auto crewCredits = holder->crewCredits();
    if (!crewCredits)
        return {};
    ranges::copy_if(*crewCredits, back_inserter(credits), [job](Credit *c) {
        return c->job() == job;
    });
    MediumCredits res;
    ranges::transform(credits, back_inserter(res), [](auto &c) {
        return MediumCredit{c->job(), Person::person(c->personId())};
    });
    if (max == 0)
        return res;
    return ranges::to<MediumCredits>(res | views::take(std::min((size_t) max, res.size())));
}

size_t sizeOfCredits(MediumDepartmentsCredits const &credits) {
    size_t res = 0;
    for (auto const &d : credits)
        res += d.credits.size();
    return res;
}

} // namespace greu::movies
