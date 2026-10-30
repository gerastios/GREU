#pragma once
#include <QString>
#include "greucore/include/greuGlobal.h"
#include <vector>

namespace greu::movies {

struct GREU_PUBLIC(MOVIES) SearchOptions {
    enum Target : int {
        targetMovie = 1,
        targetMovieCollection = targetMovie << 1,
        targetSeries = targetMovie << 2,
        targetSeason = targetMovie << 3,
        targetEpisode = targetMovie << 4,
        targetCollection = targetMovie << 5,
        targetPerson = targetMovie << 6,
    };

    enum Field : int {
        fieldTitle = 1,
        fieldDescription = fieldTitle << 1,
    };

    int target, field;
    std::vector<QString> movieGenres, seriesGenres, departments;
    QString searchString;
    bool isPattern;
};

} // namespace greu::movies
