#pragma once
#include "Cleanable.h"
#include "Searchable.h"
#include "greusql/include/Table.h"

namespace greu::core {
class Uuid;
}

namespace greu::movies {

class Episode;

class GREU_PUBLIC(MOVIES) EpisodesTable : public Cleanable, public Searchable {
public:
    static EpisodesTable *instance();
    virtual ~EpisodesTable() = default;

    Episode *read(core::Uuid const &);
    void read(Episode *);
    void save(Episode const *);

    std::set<core::Uuid> idsOfSeason(core::Uuid const &);

    virtual uint64_t clean() override;
    virtual FoundObjects search(SearchOptions const &) const override;

private:
    EpisodesTable();
    static void read(Episode *, Record const &);
};

} // namespace greu::movies
