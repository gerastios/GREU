#pragma once

// clang-format off
#define GREU_P_DEF(L, U, T) \
T const & L() const; \
    void set##U(T const & v);

#define GREU_P_DET(L, T) T L;

#define GREU_P_IMP(C, L, U, T) \
T const & C::L() const { return d->L; } \
    void C::set##U(T const & v) { d->L = v; }

// clang-format on
