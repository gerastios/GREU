#pragma once
#include "Object.h"
#include "greuui/include/ImageContainer.h"
#include "property.h"

namespace greu::movies {

class GREU_PUBLIC(MOVIES) MovieObject : public Object {
public:
    enum class ObjectType : int {
        movie = 0,
        movieCollection = 1,
        series = 2,
        season = 3,
        episode = 4,
        person = 5,
        collection = 7,
    };

    explicit MovieObject(core::Uuid const &);
    virtual ObjectType objectType() const = 0;

    virtual void setName(QString const &);
    virtual QString const &name(bool = false) const;

    virtual void update() = 0;

    GREU_P_DEF(description, Description, QString)
    GREU_P_DEF(date, Date, QDate)
    QImage const image() const;
    GREU_P_DEF(imageX, ImageX, ui::ImageContainer)
    GREU_P_DEF(fallbackUsed, FallbackUsed, bool)

private:
    virtual QImage emptyImage() const = 0;
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies
