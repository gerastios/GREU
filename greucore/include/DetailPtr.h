#pragma once

#include "greuGlobal.h"
#include <memory>
#include <type_traits>

namespace greu::core {

template<class T>
void GREU_PUBLIC(CORE) detailDelete(T *d) {
    static_assert(sizeof(T) > 0, "detailDelete cannot delete incomplete type");
    static_assert(!std::is_void<T>::value, "detailDelete cannot delete incomplete type");
    delete d;
}

template<class T, class Deleter = void (*)(T *)>
using DetailPtr = std::unique_ptr<T, Deleter>;

// Constructs an object of type T and wraps it and related default deleter in `DetailPtr`
template<class T, class... Args>
inline DetailPtr<T> GREU_PUBLIC(CORE) makeDetailPtr(Args &&...args) {
    static_assert(!std::is_array<T>::value, "DetailPtr does not support arrays");
    return DetailPtr<T>(new T(std::forward<Args>(args)...), &detailDelete<T>);
}

} // namespace greu::core

#define GREU_MKDET(ARGS...) makeDetailPtr<Detail>(ARGS)
