#pragma once
#include "Cleanable.h"
#include "Searchable.h"

namespace greu::core {
class Uuid;
}

namespace greu::movies {

class MovieCollection;

class GREU_PUBLIC(MOVIES) MovieCollectionsTable : public Cleanable, public Searchable {
public:
    static MovieCollectionsTable *instance();
    ~MovieCollectionsTable() = default;

    MovieCollection *readForTmdb(int32_t, bool = false);

    MovieCollection *read(core::Uuid const &);
    void read(MovieCollection *);
    void save(MovieCollection const *);

    std::vector<core::Uuid> allMovieCollectionIds(bool onlyMoreThaOne = false) const;

    std::set<core::Uuid> collectionsOfMovie(core::Uuid const &) const;

    virtual uint64_t clean() override;
    virtual FoundObjects search(SearchOptions const &) const override;

private:
    MovieCollectionsTable();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies
