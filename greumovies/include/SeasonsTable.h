#pragma once
#include "Cleanable.h"
#include "Searchable.h"

namespace greu::core {
class Uuid;
}

namespace greu::movies {

class Season;

class GREU_PUBLIC(MOVIES) SeasonsTable : public Cleanable, public Searchable {
public:
    static SeasonsTable *instance();
    ~SeasonsTable() = default;

    Season *readForTmdb(int32_t seriesId, int32_t, QString const &originalLanguageCode1, bool = false);

    Season *read(core::Uuid const &);
    void read(Season *);
    void save(Season const *);

    std::set<core::Uuid> idsOfSeries(core::Uuid const &);

    virtual uint64_t clean() override;
    virtual FoundObjects search(SearchOptions const &) const override;

private:
    SeasonsTable();
    static void read(Season *, Record const &);
};

} // namespace greu::movies
