#include "CreditsTable.h"
#include "CreditAssignmentsTable.h"
#include "Episode.h"
#include "Movie.h"
#include "Person.h"
#include "PersonsTable.h"
#include "Season.h"
#include "Series.h"
#include "Strings.h"
#include "creditsDefinitions.h"
#include "datatypes.h"
#include "greusql/include/Query.h"
#include "greutmdb/include/CreditRequester.h"
#include "greutmdb/include/TheMovieDatabase.h"

using namespace std;
using namespace greu::core;
using namespace greu::movies;
using namespace greu::sql;

namespace greu::movies {

struct CreditsTable::Detail {
    Credit *read(Uuid const &u, CreditsTable const *that) {
        auto ro = that->selectSingle({u});
        if (!ro)
            return nullptr;
        auto r = ro.value();
        auto cr = Object::object<Credit>(u);
        cr->setTmdbId(r[TMDB_ID].toString());
        cr->setPersonId(r[PERSON_ID]);
        cr->setCharacter(r[CHARACTER].toString());
        cr->setOrder(r[ORDER].toInt());
        cr->setJob(r[JOB].toString());
        return cr;
    }

    Credit::Credits read(vector<pair<Uuid, CreditHolder::CreditType>> const &keys,
                         CreditsTable *that) {
        Credit::Credits res;
        for (auto &k : keys) {
            auto cr = read(k.first, that);
            if (!cr)
                continue;
            switch (k.second) {
            case CreditHolder::cast:
                res.castCredits.push_back(std::move(cr));
                break;
            case CreditHolder::crew:
                res.crewCredits.push_back(std::move(cr));
                break;
            case CreditHolder::creators:
                res.creatorsCredits.push_back(std::move(cr));
                break;
            case CreditHolder::guestStars:
                res.guestStarsCredits.push_back(std::move(cr));
                break;
            }
        }
        return res;
    }

    void saveCredit(Credit const *credit, CreditsTable *that) {
        that->insertOrUpdate({credit->uuid(),
                              credit->tmdbId(),
                              credit->personId(),
                              credit->job(),
                              credit->character(),
                              credit->order()});
        PersonsTable::instance()->save(Person::person(credit->personId()));
    }
};

CreditsTable *CreditsTable::instance() {
    static CreditsTable i;
    return &i;
}

Credit::Credits CreditsTable::read(CreditHolder const &holder) {
    auto assignments = CreditAssignmentTable::instance()->read(holder.uuid());
    if (assignments.size() > 0)
        return d->read(assignments, this);
    if (!tmdb::TheMovieDatabase::isUsed())
        return {};
    auto tmdbUrlo = holder.tmdbUrl();
    if (!tmdbUrlo)
        return {};
    auto tmdb = tmdb::CreditRequester{tmdbUrlo.value()}.read();
    Credit::Credits res;
    for (auto c : tmdb.castCredits) {
        auto cr = Object::object<Credit>(Uuid{});
        cr->setTmdbId(c.id);
        auto p = Person::forTmdb(c.personId);
        cr->setPersonId(p->uuid());
        cr->setCharacter(c.character);
        cr->setOrder(c.order);
        res.castCredits.push_back(std::move(cr));
    }
    for (auto c : tmdb.guestStarsCredits) {
        auto cr = Object::object<Credit>(Uuid{});
        cr->setTmdbId(c.id);
        auto p = Person::forTmdb(c.personId);
        cr->setPersonId(p->uuid());
        cr->setCharacter(c.character);
        cr->setOrder(c.order);
        res.guestStarsCredits.push_back(std::move(cr));
    }
    for (auto c : tmdb.crewCredits) {
        auto cr = Object::object<Credit>(Uuid{});
        cr->setTmdbId(c.id);
        auto p = Person::forTmdb(c.personId);
        cr->setPersonId(p->uuid());
        cr->setJob(c.job);
        res.crewCredits.push_back(std::move(cr));
    }
    for (auto c : holder.tmdbCreatorCredits()) {
        auto cr = Object::object<Credit>(Uuid{});
        cr->setTmdbId(c.id);
        cr->setJob(CreditsTable::creatorJob());
        auto p = Person::forTmdb(c.personId);
        cr->setPersonId(p->uuid());
        res.creatorsCredits.push_back(std::move(cr));
    }
    ranges::sort(res.creatorsCredits, [](auto const &f, auto const &s) {
        return Person::person(f->personId())->name() < Person::person(s->personId())->name();
    });
    ranges::sort(res.guestStarsCredits,
                 [](auto const &f, auto const &s) { return f->order() < s->order(); });
    ranges::sort(res.castCredits,
                 [](auto const &f, auto const &s) { return f->order() < s->order(); });
    return res;
}

CreditsTable::CreditsTable()
    : Cleanable("credits",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {Strings::fieldTmdbId, GREU_MVDB_TYPE_NAME},
                 {"person_id", GREU_MVDB_TYPE_ID},
                 {"job", GREU_MVDB_TYPE_NAME},
                 {"castcharacter", GREU_MVDB_TYPE_NAME},
                 {"castorder", GREU_MVDB_TYPE_NUMBER}},
                {{"person_index", {"person_id"}}, {"tmdb_index", {Strings::fieldTmdbId}}})
    , d(GREU_MKDET()) {
    Table::create();
}

void CreditsTable::save(Credit::Credits const &credits, CreditHolder const &holder) {
    for (auto &c : credits.castCredits) {
        CreditAssignmentTable::instance()->save(c->uuid(), holder, CreditHolder::cast);
        d->saveCredit(c, this);
    }
    for (auto &c : credits.crewCredits) {
        CreditAssignmentTable::instance()->save(c->uuid(), holder, CreditHolder::crew);
        d->saveCredit(c, this);
    }
    for (auto &c : credits.creatorsCredits) {
        CreditAssignmentTable::instance()->save(c->uuid(), holder, CreditHolder::creators);
        d->saveCredit(c, this);
    }
    for (auto &c : credits.guestStarsCredits) {
        CreditAssignmentTable::instance()->save(c->uuid(), holder, CreditHolder::guestStars);
        d->saveCredit(c, this);
    }
}

Credit::BundledCredits CreditsTable::creditsOfPerson(core::Uuid const &u) const {
    Credit::BundledCredits res;
    Query q("select * from credits as c left join credit_assignments as a on c.id = a.id where "
            "c.person_id = :pid;");
    q.bindValue(":pid", u);
    q.execute();
    while (q.next()) {
        auto credit = d->read(q.value(ID), this);
        CreditHolder::CreditType type = to(q.value(ORDER + A_TYPE + 1).toInt());
        auto holderType = toHolderType(q.value(ORDER + A_MEDIUM_TYPE + 1).toInt());
        CreditHolder *holder;
        Uuid holderId = q.value(ORDER + A_MEDIUM_ID + 1);
        switch (holderType) {
        case CreditHolder::movie:
            holder = Movie::movie(holderId);
            break;
        case CreditHolder::series:
            holder = Series::series(holderId);
            break;
        case CreditHolder::season:
            holder = Season::season(holderId);
            break;
        case CreditHolder::episode:
            holder = Episode::episode(holderId);
            break;
        }
        switch (type) {
        case CreditHolder::cast:
        case CreditHolder::guestStars:
            res.castCredits.push_back({holder, type, credit});
            break;
        case CreditHolder::crew:
        case CreditHolder::creators:
            res.crewCredits.push_back({holder, type, credit});
            if (type == CreditHolder::creators)
                credit->setJob(CreditsTable::creatorJob());
            break;
        }
    }
    return res;
}

uint64_t CreditsTable::clean() {
    auto q = Query::prepareQuery(
        "select distinct id from credits where not exists (select * from credit_assignments where "
        "id = credits.id);");
    return deleteRecords(fields().at(ID), selectToDelete(q));
}

int CreditsTable::to(CreditHolder::CreditType t) {
    switch (t) {
    case CreditHolder::cast:
        return TYPE_CAST;
    case CreditHolder::crew:
        return TYPE_CREW;
    case CreditHolder::creators:
        return TYPE_CREATED_BY;
    case CreditHolder::guestStars:
        return TYPE_GUEST_STARS;
    }
    assert(false);
}

int CreditsTable::to(CreditHolder::HolderType t) {
    switch (t) {
    case CreditHolder::movie:
        return TYPE_MOVIE;
    case CreditHolder::series:
        return TYPE_SERIES;
    case CreditHolder::season:
        return TYPE_SEASON;
    case CreditHolder::episode:
        return TYPE_EPISODE;
    }
    assert(false);
}

CreditHolder::CreditType CreditsTable::to(int i) {
    switch (i) {
    case TYPE_CAST:
        return CreditHolder::cast;
    case TYPE_CREW:
        return CreditHolder::crew;
    case TYPE_CREATED_BY:
        return CreditHolder::creators;
    case TYPE_GUEST_STARS:
        return CreditHolder::guestStars;
    default:
        assert(false);
    }
}

CreditHolder::HolderType CreditsTable::toHolderType(int i) {
    switch (i) {
    case TYPE_MOVIE:
        return CreditHolder::movie;
    case TYPE_SERIES:
        return CreditHolder::series;
    case TYPE_SEASON:
        return CreditHolder::season;
    case TYPE_EPISODE:
        return CreditHolder::episode;
    }
    assert(false);
}

QString CreditsTable::creatorJob() {
    return "Creator";
}
} // namespace greu::movies
