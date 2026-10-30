#pragma once
#include <QtCore/qglobal.h>
#include <QDebug>

// clang-format off
#define GREU_NO_MOVE(CL) \
    CL(CL &&) = delete; \
    CL &operator=(CL &&) = delete;

#define GREU_MOVE(CL) \
    CL(CL &&) = default; \
    CL &operator=(CL &&) = default;

#define GREU_NO_COPY(CL) \
    CL(CL const &) = delete; \
    CL &operator=(CL const &) = delete;

#define GREU_COPY(CL) \
    CL(CL const &); \
    CL &operator=(CL const &);

#define GREU_COPY_DEF(CL) \
    CL(CL const &) = default; \
    CL &operator=(CL const &) = default;

#define GREU_COPY_IMPL(CL) \
CL::CL(CL const &other) \
    : d(GREU_MKDET(*other.d)) {} \
CL &CL::operator=(CL const &other) { \
    if (this != &other) \
        d = GREU_MKDET(*other.d); \
    return *this; \
}

#define GREU_COPY_IMPL_BASE(CL, B) \
CL::CL(CL const &other) \
    : B(), d(GREU_MKDET(*other.d)) {} \
CL &CL::operator=(CL const &other) { \
    if (this != &other) \
        d = GREU_MKDET(*other.d); \
    return *this; \
}

#define GREU_PUBLIC(M) GREU_COND(GREU##M##_LIBRARY, Q_DECL_EXPORT, Q_DECL_IMPORT)

#define GREU_COND(C, V1, V2) GREU_SELECT_THIRD(GREU_COND_COMMA(C), V1, V2)

#define GREU_COND_COMMA(...) GREU_COND_COMMA_IMPL(__VA_ARGS__, )

#define GREU_COND_COMMA_IMPL(x, ...) GREU_COND_COMMA_##x##_

#define GREU_COND_COMMA_1_ ,

#define GREU_SELECT_THIRD(...) GREU_SELECT_THIRD_IMPL(__VA_ARGS__)

#define GREU_SELECT_THIRD_IMPL(a, b, c, ...) c

#if defined(Q_DECL_HIDDEN)
#  define GREU_PRIVATE Q_DECL_HIDDEN
#else
#  define GREU_PRIVATE
#endif
// clang-format on

