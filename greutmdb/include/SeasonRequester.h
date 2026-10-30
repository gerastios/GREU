#pragma once
#include "Credit.h"
#include "TmdbRequester.h"
#include "greuui/include/ImageContainer.h"

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) SeasonRequester : public TmdbRequester {
public:
    struct Data {
        int32_t seriesId, number;
        QString name, overview, originalName{};
        QDate airDate;
        ui::ImageContainer poster{};
        bool fallbackUsed{false};
    };

    SeasonRequester(int32_t seriesId, int32_t, QString const &originalLanguageCode1);

    virtual QUrl url() const override;
    Data read();

    QUrl creditUrl() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tmdb
