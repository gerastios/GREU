#pragma once
#include <QUrlQuery>
#include "Requester.h"
#include "greucore/include/DetailPtr.h"

namespace greu::ui {
class ImageContainer;
}

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) ImageReader : public Requester {
public:
    ImageReader(QString const &fileName, bool thumbnail);
    virtual QUrl url() const override;
    inline virtual QUrlQuery query() const override { return QUrlQuery{}; }
    inline virtual bool isApiKeyNeeded() const override { return false; }
    inline virtual QString acceptString() const override { return ""; }
    virtual void finished(QNetworkReply *) override;
    ui::ImageContainer read();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tmdb
