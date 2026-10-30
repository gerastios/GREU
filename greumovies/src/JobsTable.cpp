#include "JobsTable.h"
#include "datatypes.h"
#include "greusql/include/Query.h"
#include "greutmdb/include/TheMovieDatabase.h"

using namespace std;
using namespace greu::core;
using namespace greu::movies;
using namespace greu::sql;
using namespace greu::tmdb;

namespace {
// clang-format off
static int constexpr SLINE = __LINE__ + 1,
    JOB = __LINE__ - SLINE,
    DEPARTMENT = __LINE__ - SLINE;
// clang-format on

} // namespace

namespace greu::movies {

JobsTable::JobsTable()
    : Table("jobs",
            {{"job", GREU_MVDB_TYPE_NAME, true}, {"department", GREU_MVDB_TYPE_NAME}},
            {{"department_index", {"department"}}}) {
    Table::create();
}

JobsTable *JobsTable::instance() {
    static JobsTable i;
    return &i;
}

void JobsTable::refresh() {
    deleteAll();
    auto jobs = tmdb::TheMovieDatabase::instance()->jobs();
    for (auto &j : jobs)
        insertOrUpdate({j.name, j.department});
}

set<QString> JobsTable::allJobs() const {
    auto recs = selectAll();
    decltype(allJobs()) res;
    for (auto &r : recs)
        res.insert(r[JOB].toString());
    return res;
}

set<QString> JobsTable::allDepartments() const {
    Query q{"select distinct department from jobs;"};
    q.execute();
    decltype(allDepartments()) res;
    while (q.next())
        res.insert(q.value(0).toString());
    return res;
}

QString JobsTable::departmentOfJob(QString const &j) const {
    auto ro = selectSingle({j});
    if (ro)
        return ro.value()[DEPARTMENT].toString();
    return {};
}

} // namespace greu::movies
