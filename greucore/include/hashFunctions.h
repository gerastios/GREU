#pragma once
#include "greuGlobal.h"
#include <boost/container_hash/hash.hpp>

namespace std {

template<>
struct GREU_PUBLIC(CORE) hash<std::pair<int32_t, int32_t>> {
    inline size_t operator()(std::pair<int32_t, int32_t> const &t) const {
        size_t seed = 0;
        boost::hash_combine<int32_t>(seed, t.first);
        boost::hash_combine<int32_t>(seed, t.second);
        return seed;
    }
};

template<>
struct GREU_PUBLIC(CORE) hash<std::tuple<int32_t, int32_t, int32_t>> {
    inline size_t operator()(std::tuple<int32_t, int32_t, int32_t> const &t) const {
        size_t seed = 0;
        boost::hash_combine<int32_t>(seed, std::get<0>(t));
        boost::hash_combine<int32_t>(seed, std::get<1>(t));
        boost::hash_combine<int32_t>(seed, std::get<2>(t));
        return seed;
    }
};

} // namespace std
