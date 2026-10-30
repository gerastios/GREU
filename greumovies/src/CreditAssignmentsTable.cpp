#include "CreditAssignmentsTable.h"
#include "CreditsTable.h"
#include "Strings.h"
#include "creditsDefinitions.h"
#include "datatypes.h"
#include "greusql/include/Query.h"

using namespace std;

namespace greu::movies {

using namespace sql;
using namespace core;

CreditAssignmentTable::CreditAssignmentTable()
    : Cleanable("credit_assignments",
                {{Strings::fieldId, GREU_MVDB_TYPE_ID, true},
                 {"medium_id", GREU_MVDB_TYPE_ID, true},
                 {"medium_type", GREU_MVDB_TYPE_TYPE, true},
                 {"type", GREU_MVDB_TYPE_TYPE, true}},
                {{"medium_index", {"medium_id"}}}) {
    Table::create();
}

uint64_t CreditAssignmentTable::deleteType(QString const &t, int type) {
    auto q = Query::prepareQuery(
        QString("select distinct id from credit_assignments where medium_type = :mt and not "
                "exists (select * from %1 where "
                "id = credit_assignments.medium_id);")
            .arg(t));
    q.bindValue(":mt", type);
    return deleteRecords(fields().at(A_ID), selectToDelete(q));
}

CreditAssignmentTable *CreditAssignmentTable::instance() {
    static CreditAssignmentTable i;
    return &i;
}

void CreditAssignmentTable::save(Uuid const &uuid,
                                 CreditHolder const &holder,
                                 CreditHolder::CreditType t) {
    insertOrUpdate(
        {uuid, holder.uuid(), CreditsTable::to(holder.creditHolderType()), CreditsTable::to(t)});
}

uint64_t CreditAssignmentTable::clean() {
    auto res = deleteType("series", TYPE_SERIES);
    res += deleteType("movies", TYPE_MOVIE);
    res += deleteType("seasons", TYPE_SEASON);
    res += deleteType("episodes", TYPE_EPISODE);
    return res;
}

vector<pair<Uuid, CreditHolder::CreditType>> CreditAssignmentTable::read(Uuid const &mi) {
    auto r = selectEqual({mi}, {&fields()[A_MEDIUM_ID]});
    decltype(read({})) res;
    for (auto c : r)
        res.push_back({Uuid(c[A_ID]), CreditsTable::to(c[A_TYPE].toInt())});
    return res;
}

} // namespace greu::movies
