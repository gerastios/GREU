#include "MovieObject.h"
#include <QDate>
#include <QImage>

namespace greu::movies {

using namespace core;

struct MovieObject::Detail {
    GREU_P_DET(name, QString)
    GREU_P_DET(description, QString)
    GREU_P_DET(date, QDate)
    GREU_P_DET(imageX, ui::ImageContainer)
    GREU_P_DET(fallbackUsed, bool)
};

MovieObject::MovieObject(Uuid const &u)
    : Object(u)
    , d(GREU_MKDET()) {}

void MovieObject::setName(QString const &v) {
    d->name = v;
}

QString const &MovieObject::name(bool) const {
    return d->name;
}

QImage const MovieObject::image() const {
    return d->imageX.isEmpty() ? emptyImage() : d->imageX.toImage();
}

GREU_P_IMP(MovieObject, description, Description, QString)
GREU_P_IMP(MovieObject, date, Date, QDate)
GREU_P_IMP(MovieObject, imageX, ImageX, ui::ImageContainer)
GREU_P_IMP(MovieObject, fallbackUsed, FallbackUsed, bool)

} // namespace greu::movies
