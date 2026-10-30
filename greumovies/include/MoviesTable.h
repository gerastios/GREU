#pragma once
#include "Cleanable.h"
#include "Searchable.h"

namespace greu::core {
class Uuid;
}

namespace greu::movies {

class Movie;
struct SearchOptions;

class GREU_PUBLIC(MOVIES) MoviesTable : public Cleanable, public Searchable {
public:
    static MoviesTable *instance();
    ~MoviesTable() = default;

    Movie *read(core::Uuid const &);
    void read(Movie *);
    void save(Movie const *);

    std::vector<core::Uuid> allMovieIds() const;
    std::set<core::Uuid> movieIdsOfMovieCollection(core::Uuid const &) const;
    std::set<core::Uuid> idsOfIndependentMovies() const;

    virtual uint64_t clean() override;
    virtual FoundObjects search(SearchOptions const &) const override;

private:
    MoviesTable();
    static void read(Movie *, Record const &);
};

} // namespace greu::movies
