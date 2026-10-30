#pragma once
#include "Credit.h"
#include "TmdbRequester.h"
#include "greuui/include/ImageContainer.h"

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) SeriesRequester : public TmdbRequester {
public:
    struct Data {
        int32_t id;
        std::set<QString> genres;
        QString name, originalName, originalLanguage, originalLanguageCode, overview, tagline;
        QDate firstAirDate, lastAirDate;
        ui::ImageContainer poster{};
        std::vector<Credit> creators{};
        bool fallbackUsed{false};
    };

    SeriesRequester(int32_t);

    virtual QUrl url() const override;
    Data read();

    QUrl creditUrl() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tmdb
