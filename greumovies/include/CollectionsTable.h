#pragma once
#include "Cleanable.h"
#include "Collectable.h"
#include "Searchable.h"

namespace greu::movies {

class Collection;

class GREU_PUBLIC(MOVIES) CollectionsTable : public Cleanable, public Searchable {
public:
    static CollectionsTable *instance();
    ~CollectionsTable() = default;

    Collection *read(core::Uuid const &);
    void read(Collection *);
    void save(Collection const *);

    std::set<core::Uuid> allCollectionIds() const;
    std::vector<core::Uuid> allTopCollectionIds(bool onlyMoreThaOne = false) const;
    std::set<core::Uuid> collectionIdsOfCollectable(Collectable::Key const &) const;

    virtual uint64_t clean() override;
    virtual std::set<MovieObject *> search(SearchOptions const &) const override;

private:
    CollectionsTable();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies
