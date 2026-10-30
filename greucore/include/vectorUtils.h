#pragma once

#include "greuGlobal.h"
#include <algorithm>
#include <optional>
#include <vector>

namespace greu::core {

template<typename T>
bool GREU_PUBLIC(CORE) contains(std::vector<T> const &vec, T const &value) {
    return std::find(vec.cbegin(), vec.cend(), value) != vec.cend();
}

template<typename T>
std::optional<T> GREU_PUBLIC(CORE) find(std::vector<T> const &vec, T const &value) {
    auto e = std::find(vec.begin(), vec.end(), value);
    if (e == vec.end())
        return std::nullopt;
    return *e;
}

template<typename T>
std::optional<size_t> GREU_PUBLIC(CORE) indexOf(std::vector<T> const &vec, T const &value) {
    auto e = std::find(vec.begin(), vec.end(), value);
    if (e == vec.end())
        return std::nullopt;
    return e - vec.begin();
}

template<typename T>
void GREU_PUBLIC(CORE) erase(std::vector<T> &vec, T const &value) {
    auto e = std::find(vec.begin(), vec.end(), value);
    if (e != vec.end())
        vec.erase(e);
}

template<typename T>
void GREU_PUBLIC(CORE) insertSortedIndex(std::vector<T> const &vec,
                                         std::vector<size_t> &indexes,
                                         T const &value,
                                         size_t index) {
    size_t i = 0;
    for (auto idx : indexes) {
        if (vec.at(idx) > value) {
            indexes.insert(std::next(indexes.begin(), i), index);
            return;
        }
        i++;
    }
    indexes.push_back(index);
}

template<typename T>
void GREU_PUBLIC(CORE)
    removeSortedIndex(std::vector<T> const &vec, std::vector<size_t> &indexes, T const &value) {
    size_t index = 0;
    bool found = false;
    for (auto idx = vec.begin(); idx != vec.end() && !found; idx++) {
        if (*idx == value)
            found = true;
        else
            index++;
    }
    erase(indexes, index);
    for (auto v = indexes.begin(); v != indexes.end(); v++)
        if (*v > index)
            (*v)--;
}

template<typename T>
void GREU_PUBLIC(CORE) move(std::vector<T> &vec, int from, int to) {
    auto e = std::move(vec[from]);
    vec.erase(vec.cbegin() + from);
    vec.insert(vec.cbegin() + to, std::move(e));
}

} // namespace greu::core
