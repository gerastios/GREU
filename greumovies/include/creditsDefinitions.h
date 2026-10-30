#pragma once
#include <cstddef>

namespace {
#define TYPE_CAST 1
#define TYPE_CREW 2
#define TYPE_CREATED_BY 4
#define TYPE_GUEST_STARS 8

#define TYPE_MOVIE 1
#define TYPE_SERIES 2
#define TYPE_SEASON 4
#define TYPE_EPISODE 8

// clang-format off
static size_t constexpr SLINE = __LINE__ + 1,
    ID = __LINE__ - SLINE,
    TMDB_ID = __LINE__ - SLINE,
    PERSON_ID = __LINE__ - SLINE,
    JOB = __LINE__ - SLINE,
    CHARACTER = __LINE__ - SLINE,
    ORDER = __LINE__ - SLINE;
// clang-format on

// clang-format off
static size_t constexpr A_SLINE = __LINE__ + 1,
    A_ID = __LINE__ - A_SLINE,
    A_MEDIUM_ID = __LINE__ - A_SLINE,
    A_MEDIUM_TYPE = __LINE__ - A_SLINE,
    A_TYPE = __LINE__ - A_SLINE;
// clang-format on
} // namespace
