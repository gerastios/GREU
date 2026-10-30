#pragma once
#include "Cleanable.h"
#include "SearchOptions.h"
#include "Searchable.h"

namespace greu::core {
class Uuid;
}

namespace greu::movies {

class Person;

class GREU_PUBLIC(MOVIES) PersonsTable : public Cleanable, public Searchable {
public:
    static PersonsTable *instance();
    ~PersonsTable() = default;

    Person *readForTmdb(int32_t, bool = false);

    Person *read(core::Uuid const &);
    void read(Person *);
    void save(Person const *);

    virtual uint64_t clean() override;
    virtual FoundObjects search(SearchOptions const &) const override;

private:
    PersonsTable();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies
