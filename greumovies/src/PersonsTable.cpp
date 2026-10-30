#include "PersonsTable.h"
#include <QImage>
#include "Person.h"
#include "Strings.h"
#include "datatypes.h"
#include "greucore/include/Uuid.h"
#include "greucore/include/vectorUtils.h"
#include "greusql/include/Query.h"
#include "greutmdb/include/PersonRequester.h"
#include "searchUtils.h"

using namespace std;
using namespace greu::core;
using namespace greu::movies;
using namespace greu::sql;
using namespace greu::ui;

namespace {
// clang-format off
static int constexpr SLINE = __LINE__ + 1,
    ID = __LINE__ - SLINE,
    TMDB_ID = __LINE__ - SLINE,
    NAME = __LINE__ - SLINE,
    BIOGRAPHY = __LINE__ - SLINE,
    KNOWN_FOR_JOB_DEPARTMENT = __LINE__ - SLINE,
    PLACE_OF_BIRTH = __LINE__ - SLINE,
    BIRTHDAY = __LINE__ - SLINE,
    DEATHDAY = __LINE__ - SLINE,
    FALLBACK_USED = __LINE__ - SLINE,
    IMAGE = __LINE__ - SLINE,
    IMAGE_TYPE = __LINE__ - SLINE;
// clang-format on

// clang-format off
static int constexpr A_SLINE = __LINE__ + 1,
    A_ID = __LINE__ - A_SLINE,
    A_ALIAS = __LINE__ - A_SLINE;
// clang-format on

struct AliasTable : public Table {
    AliasTable()
        : Table("person_aliases",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true}, {"alias", GREU_MVDB_TYPE_NAME, true}},
                {{"alias_index", {"alias"}}}) {
        create();
    }

    set<QString> read(Uuid const &u) const {
        set<QString> res;
        auto rs = selectEqual({u}, {&fields()[A_ID]});
        for (auto &r : rs)
            res.insert(r[A_ALIAS].toString());
        return res;
    }

    void save(Person const *p) {
        Records recs;
        for (auto &a : p->aliases())
            recs.push_back({p->uuid(), a});
        deleteAndInsert({p->uuid()}, {&fields()[A_ID]}, recs);
    }
};

QString const ActorsDep{"Actors"};

} // namespace

namespace greu::movies {

struct PersonsTable::Detail {
    AliasTable aliasTable;

    void read(Person *no, Record const &r) {
        no->setTmdbId(r[TMDB_ID].toInt());
        no->setName(r[NAME].toString());
        no->setDescription(r[BIOGRAPHY].toString());
        no->setKnownForJobDepartment(r[KNOWN_FOR_JOB_DEPARTMENT].toString());
        no->setPlaceOfBirth(r[PLACE_OF_BIRTH].toString());
        no->setDate(r[BIRTHDAY].toDate());
        no->setDeathday(r[DEATHDAY].toDate());
        no->setAliases(aliasTable.read(r[ID]));
        no->setImageX(
            ImageContainer{r[IMAGE].toByteArray(), (ImageContainer::Type) r[IMAGE_TYPE].toInt()});
        no->setFallbackUsed(r[FALLBACK_USED].toInt());
    }
};

PersonsTable::PersonsTable()
    : Cleanable("persons",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {Strings::fieldTmdbId, GREU_MVDB_TYPE_NUMBER},
                 {Strings::fieldName, GREU_MVDB_TYPE_NAME},
                 {"biography", GREU_MVDB_TYPE_DESCRIPTION},
                 {"known_for_job_department", GREU_MVDB_TYPE_NAME},
                 {"place_of_birth", GREU_MVDB_TYPE_LOCATION},
                 {"birthday", GREU_MVDB_TYPE_DATE},
                 {"deathday", GREU_MVDB_TYPE_DATE},
                 {"fallback_used", GREU_MVDB_TYPE_BOOLEAN},
                 {"image", GREU_MVDB_TYPE_IMAGE},
                 {"image_type", GREU_MVDB_TYPE_TYPE}},
                {{"tmdb_index", {Strings::fieldTmdbId}}})
    , d(GREU_MKDET()) {
    Table::create();
}

PersonsTable *PersonsTable::instance() {
    static PersonsTable i;
    return &i;
}

Person *PersonsTable::readForTmdb(int32_t tmdbId, bool update) {
    auto r = selectEqual({tmdbId}, {&fields()[TMDB_ID]});
    Person *no = nullptr;
    if (r.size() == 1) {
        if (Object::exists(r[0][ID]))
            no = Person::person(r[0][ID]);
        else
            no = read(r[0][ID]);
    }
    if (no && !update)
        return no;
    auto od = tmdb::PersonRequester(tmdbId).read();
    if (!no) {
        no = Object::object<Person>(Uuid{});
        no->setTmdbId(od.id);
    }
    if (no->name().isEmpty())
        no->setName(od.name);
    if ((no->description().isEmpty() || no->fallbackUsed()) && !od.biography.isEmpty())
        no->setDescription(od.biography);
    if (no->knownForJobDepartment().isEmpty())
        no->setKnownForJobDepartment(od.knownForDepartment);
    if (no->placeOfBirth().isEmpty())
        no->setPlaceOfBirth(od.placeOfBirth);
    if (!no->date().isValid())
        no->setDate(od.birthday);
    if (!no->deathday().isValid())
        no->setDeathday(od.deathday);
    if (od.alsoKnownAs.size() > 0)
        no->setAliases(od.alsoKnownAs);
    if (od.image.isValid())
        no->setImageX(od.image);
    no->setFallbackUsed(od.fallbackUsed);
    no->setNew();
    return no;
}

Person *PersonsTable::read(core::Uuid const &u) {
    return Object::object<Person>(u);
}

void PersonsTable::read(Person *o) {
    auto ro = selectSingle({o->uuid()});
    if (!ro)
        return;
    auto r = ro.value();
    d->read(o, r);
}

void PersonsTable::save(Person const *o) {
    if (!o->isNew())
        return;
    insertOrUpdate({o->uuid(),
                    o->tmdbId(),
                    o->name(),
                    o->description(),
                    o->knownForJobDepartment(),
                    o->placeOfBirth(),
                    o->date(),
                    o->deathday(),
                    (int) o->fallbackUsed(),
                    o->imageX().data(),
                    (int) o->imageX().type()});
    d->aliasTable.save(o);
}

uint64_t PersonsTable::clean() {
    auto q = Query::prepareQuery(
        "select distinct id from persons where not exists (select * from credits where "
        "person_id = persons.id);");
    auto res = deleteRecords(fields().at(ID), selectToDelete(q));
    q = Query::prepareQuery(
        "delete from person_aliases where not exists (select * from persons where "
        "id = persons.id);");
    q.execute();
    return res + rowCount();
}

FoundObjects PersonsTable::search(SearchOptions const &options) const {
    auto where = buildTargetWhere(options, name(), Strings::fieldName, Strings::fieldBiography);
    QString actorWhere;
    if (contains(options.departments, ActorsDep)) {
        actorWhere = "exists (select * from credits where person_id = persons.id and job is null)";
    }
    QString depWhere;
    if (options.departments.size() > (actorWhere.isEmpty() ? 0 : 1)) {
        depWhere = "exists (select * from credits join jobs on jobs.job = credits.job where "
                   "credits.person_id = persons.id and jobs.department in (";
        for (int i = 0; auto &d : options.departments) {
            if (d == ActorsDep)
                continue;
            if (i > 0)
                depWhere += ", ";
            depWhere += QString("\"%1\"").arg(d);
            ++i;
        }
        depWhere += "))";
    }
    if (!(actorWhere.isEmpty() || depWhere.isEmpty()))
        where = QString("%1 and (%2 or %3)").arg(where).arg(actorWhere).arg(depWhere);
    else if (!actorWhere.isEmpty())
        where = QString("%1 and (%2)").arg(where).arg(actorWhere);
    else if (!depWhere.isEmpty())
        where = QString("%1 and (%2)").arg(where).arg(depWhere);
    auto qs = QString("select distinct id from persons where %1").arg(where);
    Query q{qs};
    q.bindValue(":string", options.searchString);
    q.execute();
    decltype(search({})) res;
    while (q.next())
        res.insert(Object::object<Person>(q.value(0)));
    return res;
}

} // namespace greu::movies
