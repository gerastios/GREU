#pragma once
#include "Cleanable.h"
#include "Searchable.h"

namespace greu::core {
class Uuid;
}

namespace greu::movies {

class Series;
struct SearchOptions;

class GREU_PUBLIC(MOVIES) SeriesTable : public Cleanable, public Searchable {
public:
    static SeriesTable *instance();
    ~SeriesTable() = default;

    Series *readForTmdb(int32_t, bool = false);

    Series *read(core::Uuid const &);
    void read(Series *);
    void save(Series const *);

    std::vector<core::Uuid> allSeriesIds() const;

    virtual uint64_t clean() override;
    virtual FoundObjects search(SearchOptions const &) const override;

private:
    SeriesTable();
    static void read(Series *, Record const &);
};

} // namespace greu::movies
