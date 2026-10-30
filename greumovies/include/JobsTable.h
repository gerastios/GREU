#pragma once
#include "greusql/include/Table.h"

namespace greu::movies {

class GREU_PUBLIC(MOVIES) JobsTable : public sql::Table {
public:
    static JobsTable *instance();

    void refresh();

    std::set<QString> allJobs() const;
    std::set<QString> allDepartments() const;
    QString departmentOfJob(QString const &) const;

private:
    JobsTable();
};

} // namespace greu::movies
