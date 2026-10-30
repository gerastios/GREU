#include "CollectionsTable.h"
#include <QImage>
#include "Collection.h"
#include "EmptyImages.h"
#include "EpisodesTable.h"
#include "MovieCollectionsTable.h"
#include "MoviesTable.h"
#include "SeriesTable.h"
#include "Strings.h"
#include "datatypes.h"
#include "greucore/include/Uuid.h"
#include "greusql/include/Query.h"
#include "greuui/include/ImageContainer.h"
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
    TITLE = __LINE__ - SLINE,
    DESCRIPTION = __LINE__ - SLINE,
    SORT_ORDER = __LINE__ - SLINE,
    IMAGE = __LINE__ - SLINE,
    IMAGE_TYPE = __LINE__ - SLINE;
// clang-format on

// clang-format off
static int constexpr M_SLINE = __LINE__ + 1,
    M_ID = __LINE__ - M_SLINE,
    M_COLLECTABLE_ID = __LINE__ - M_SLINE,
    M_TYPE = __LINE__ - M_SLINE;
// clang-format on

struct CollectableIdsTable : public Table {
    CollectableIdsTable()
        : Table("collectable_ids",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {"collectable_id", GREU_MVDB_TYPE_ID, true},
                 {"ctype", GREU_MVDB_TYPE_TYPE}},
                {{"collectable_index", {"collectable_id"}}}) {
        create();
    }

    vector<Collectable::Key> read(Uuid const &u) const {
        decltype(read({})) res;
        auto rs = selectEqual({u}, {&fields()[M_ID]});
        for (auto &r : rs)
            res.push_back({r[M_COLLECTABLE_ID], (Collectable::Type) r[M_TYPE].toInt()});
        return res;
    }

    void save(Collection const *c) {
        Records recs;
        for (auto &a : c->collectableKeys())
            recs.push_back({c->uuid(), a.uuid, a.type});
        deleteAndInsert({c->uuid()}, {&fields()[M_ID]}, recs);
    }

    void deleteCollection(Collection const *c) { deleteEqual({c->uuid()}, {&fields()[M_ID]}); }

    set<Uuid> collectionIdsOfCollectable(Collectable::Key const &k) const {
        auto recs = selectEqual({k.uuid, (int) k.type},
                                {&fields()[M_COLLECTABLE_ID], &fields()[M_TYPE]});
        decltype(collectionIdsOfCollectable({})) res;
        for (auto &r : recs)
            res.insert(r[M_ID]);
        return res;
    }

    uint64_t _rowCount() const { return rowCount(); }
};

uint64_t cleanType(Table const &table, int type, CollectableIdsTable const *const ids) {
    auto q = Query::prepareQuery(
        QString("delete from collectable_ids where ctype = :type and not exists (select * from %1 "
                "where id = collectable_ids.collectable_id);")
            .arg(table.name()));
    q.bindValue(":type", type);
    q.execute();
    return ids->_rowCount();
}

} // namespace

namespace greu::movies {

struct CollectionsTable::Detail {
    CollectableIdsTable idsTable;

    void read(Collection *no, Record const &r) {
        no->setName(r[TITLE].toString());
        no->setDescription(r[DESCRIPTION].toString());
        no->setSortOrder((Collection::SortOrder) r[SORT_ORDER].toInt());
        for (auto &mi : idsTable.read(no->uuid()))
            no->addCollectableKey(mi);
        no->setImageX(
            ImageContainer{r[IMAGE].toByteArray(), (ImageContainer::Type) r[IMAGE_TYPE].toInt()});
    }
};

CollectionsTable::CollectionsTable()
    : Cleanable("collections",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {Strings::fieldTitle, GREU_MVDB_TYPE_NAME},
                 {Strings::fieldDescription, GREU_MVDB_TYPE_DESCRIPTION},
                 {"sort_order", GREU_MVDB_TYPE_NUMBER},
                 {"image", GREU_MVDB_TYPE_IMAGE},
                 {"image_type", GREU_MVDB_TYPE_TYPE}})
    , d(GREU_MKDET()) {
    Table::create();
}

CollectionsTable *CollectionsTable::instance() {
    static CollectionsTable i;
    return &i;
}

Collection *CollectionsTable::read(core::Uuid const &u) {
    auto ro = selectSingle({u});
    assert(ro);
    auto r = ro.value();
    auto no = Object::object<Collection>(r[ID]);
    d->read(no, r);
    return no;
}

void CollectionsTable::read(Collection *no) {
    auto ro = selectSingle({no->uuid()});
    if (!ro)
        return;
    auto r = ro.value();
    d->read(no, r);
}

void CollectionsTable::save(Collection const *o) {
    if (!o->isNew())
        return;
    if (o->isDeleted()) {
        deleteSingle({o->uuid()});
        d->idsTable.deleteCollection(o);
    } else {
        insertOrUpdate({o->uuid(),
                        o->name(),
                        o->description(),
                        (int) o->sortOrder(),
                        o->imageX().data(),
                        (int) o->imageX().type()});
        d->idsTable.save(o);
    }
}

uint64_t CollectionsTable::clean() {
    uint64_t res = 0;
    res += cleanType(*MoviesTable::instance(), Collectable::movie, &d->idsTable);
    res += cleanType(*MovieCollectionsTable::instance(), Collectable::movieCollection, &d->idsTable);
    res += cleanType(*SeriesTable::instance(), Collectable::series, &d->idsTable);
    res += cleanType(*EpisodesTable::instance(), Collectable::episode, &d->idsTable);
    res += cleanType(*CollectionsTable::instance(), Collectable::collection, &d->idsTable);
    {
        auto q = Query::prepareQuery(
            "delete from collectable_ids where not exists (select * from collections where "
            "id = collectable_ids.id);");
        q.execute();
        res += rowCount();
    }
    return res;
}

FoundObjects CollectionsTable::search(SearchOptions const &options) const {
    return simpleSearch<Collection>(options, "collections");
}

vector<Uuid> CollectionsTable::allTopCollectionIds(bool onlyMoreThaOne) const {
    Records rr;
    if (onlyMoreThaOne) {
        rr = selectFromQuery(
            "select * from collections where (select count(*) from "
            "collectable_ids where id = collections.id) > 1 and not exists (select * from "
            "collectable_ids where collectable_id = collections.id");
    } else {
        rr = selectFromQuery("select * from collections where not exists (select * from "
                             "collectable_ids where collectable_id = collections.id)");
    }
    vector<Uuid> res;
    ranges::transform(rr, back_inserter(res), [](auto &r) { return r[ID]; });
    return res;
}

set<Uuid> CollectionsTable::allCollectionIds() const {
    auto rr = selectAll();
    set<Uuid> res;
    for (auto r : rr)
        res.insert(r[ID]);
    return res;
}

set<Uuid> CollectionsTable::collectionIdsOfCollectable(Collectable::Key const &k) const {
    return d->idsTable.collectionIdsOfCollectable(k);
}

} // namespace greu::movies
