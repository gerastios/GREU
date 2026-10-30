#pragma once
#include "greucore/include/greuGlobal.h"

namespace greu::movies {

struct SearchOptions;
class MovieObject;

using FoundObjects = std::set<MovieObject *>;

struct GREU_PUBLIC(MOVIES) Searchable {
    virtual ~Searchable() = default;
    virtual FoundObjects search(SearchOptions const &) const = 0;
};

} // namespace greu::movies
