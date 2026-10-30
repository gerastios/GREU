#pragma once
#include "TmdbRequester.h"
#include "greuui/include/ImageContainer.h"

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) PersonRequester : public TmdbRequester {
public:
    struct Data {
        int32_t id;
        QString name, placeOfBirth, knownForDepartment, biography;
        std::set<QString> alsoKnownAs;
        QDate birthday, deathday;
        ui::ImageContainer image{};
        bool fallbackUsed{false};
    };

    PersonRequester(int32_t);

    virtual QUrl url() const override;
    Data read();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::tmdb
