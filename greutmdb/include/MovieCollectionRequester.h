#pragma once
#include "TmdbRequester.h"
#include "greuui/include/ImageContainer.h"

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) MovieCollectionRequester : public TmdbRequester {
public:
    struct Data {
        int32_t id;
        QString name, originalLanguage, originalLanguageCode, originalName, overview, tagline;
        QDate releaseDate{};
        ui::ImageContainer poster{};
        std::set<int32_t> movieIds{};
        bool fallbackUsed{false};
    };

    MovieCollectionRequester(int32_t);

    virtual QUrl url() const override;
    Data read();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tmdb
