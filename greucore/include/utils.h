#pragma once

#include <type_traits>

namespace greu::core {

template<class T, class S>
concept Derived = std::is_base_of<S, T>::value;

template<class T, class S>
    requires Derived<T, S>
T *safeCast(S *o) {
    if (!o)
        return nullptr;
    auto res = dynamic_cast<T *>(o);
    assert(res);
    return res;
}

} // namespace greu::core
