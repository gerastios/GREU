#pragma once
#include "TmdbRequester.h"
#include "greuui/include/ImageContainer.h"

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) MovieRequester : public TmdbRequester {
public:
    struct Data {
        int32_t id, movieCollectionId;
        std::set<QString> genres;
        QString title, originalLanguage, originalLanguageCode, originalTitle, overview, tagline;
        QDate releaseDate;
        ui::ImageContainer poster{};
        bool fallbackUsed{false};
    };

    MovieRequester(int32_t);

    virtual QUrl url() const override;
    Data read();

    QUrl creditUrl() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tmdb
