#pragma once
#include "Credit.h"
#include "TmdbRequester.h"
#include "greuui/include/ImageContainer.h"

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) EpisodeRequester : public TmdbRequester {
public:
    struct Data {
        int32_t seriesId, seasonNumber, number;
        QString name, overview, originalName{};
        QDate airDate;
        ui::ImageContainer stillImage{};
        bool fallbackUsed{false};
    };

    EpisodeRequester(int32_t seriesId,
                     int32_t seasonNumber,
                     int32_t,
                     QString const &originalLanguageCode1);

    virtual QUrl url() const override;
    Data read();

    QUrl creditUrl() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tmdb
