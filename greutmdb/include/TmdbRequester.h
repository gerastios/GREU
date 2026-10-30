#pragma once
#include <QJsonObject>
#include <QUrlQuery>
#include "JsonRequester.h"

class QUrl;

namespace greu::core {
class Language;
}

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) TmdbRequester : public JsonRequester {
public:
    TmdbRequester();
    virtual ~TmdbRequester() = default;

    virtual QUrlQuery query() const override;

protected:
    struct Fallback {
        Fallback(TmdbRequester *);
        Fallback(TmdbRequester *, QString const &languageCode1);
        ~Fallback();

    private:
        struct Detail;
        core::DetailPtr<Detail> d;
    };

    QJsonObject readTmdb();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tmdb
