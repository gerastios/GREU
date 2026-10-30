#pragma once
#include <QString>
#include "greucore/include/greuGlobal.h"

namespace greu::movies {

struct GREU_PUBLIC(MOVIES) Strings {
    Strings() = delete;

    static QString const fieldId;
    static QString const fieldTmdbId;
    static QString const fieldTitle;
    static QString const fieldName;
    static QString const fieldDescription;
    static QString const fieldOverview;
    static QString const fieldBiography;
};

} // namespace greu::movies
