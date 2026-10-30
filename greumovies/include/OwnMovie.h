#pragma once
#include <QDate>
#include <QString>
#include "greucore/include/greuGlobal.h"
#include "greuui/include/ImageContainer.h"

namespace greu::movies {

struct GREU_PUBLIC(MOVIES) OwnPerson {
    int32_t tmdbId;
    QString name, biography;
    QDate birthDate, deathDate;
    ui::ImageContainer image;
};

struct GREU_PUBLIC(MOVIES) OwnCast {
    OwnPerson person;
    QString character;
    int order;
};

struct GREU_PUBLIC(MOVIES) OwnCrew {
    OwnPerson person;
    QString job;
};

struct GREU_PUBLIC(MOVIES) OwnMovie {
    QString title, tagline, originalTitle, description, originalLanguageCode;
    std::vector<QString> genres;
    QDate releaseDate;
    ui::ImageContainer image;
    std::vector<OwnCast> cast;
    std::vector<OwnCrew> crew;
};

} // namespace greu::movies
