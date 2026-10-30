#include "TmdbRequester.h"
#include "Reader.h"
#include "TheMovieDatabase.h"
#include "greucore/include/Language.h"
#include "greunet/include/Exception.h"

namespace greu::tmdb {

using namespace core;

struct TmdbRequester::Detail {
    QString fallbackLanguage{};
};

struct TmdbRequester::Fallback::Detail {
    TmdbRequester *objReader;
};

TmdbRequester::TmdbRequester()
    : d(GREU_MKDET()) {}

QUrlQuery TmdbRequester::query() const {
    QUrlQuery res;
    if (!d->fallbackLanguage.isEmpty())
        res.addQueryItem("language", d->fallbackLanguage);
    else
        res.addQueryItem("language", GREU_TMDB->language().code1().toLocal8Bit());
    return res;
}

QJsonObject TmdbRequester::readTmdb() {
    Reader r;
    try {
        return readJson(&r).object();
    } catch (net::Exception &e) {
        if (e.code() == QNetworkReply::NetworkError::ContentNotFoundError
            && GREU_TMDB->fallbackLanguage() != GREU_TMDB->language()) {
            Fallback f(this);
            return readJson(&r).object();
        }
        throw;
    }
}

TmdbRequester::Fallback::Fallback(TmdbRequester *r)
    : d(GREU_MKDET()) {
    d->objReader = r;
    r->d->fallbackLanguage = GREU_TMDB->fallbackLanguage().code1().toLocal8Bit();
}

TmdbRequester::Fallback::Fallback(TmdbRequester *r, QString const &languageCode1)
    : d(GREU_MKDET()) {
    d->objReader = r;
    r->d->fallbackLanguage = languageCode1;
}

TmdbRequester::Fallback::~Fallback() {
    d->objReader->d->fallbackLanguage = "";
}

} // namespace greu::tmdb
