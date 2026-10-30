#pragma once
#include "greucore/include/DetailPtr.h"

namespace greu::gaion {

class GREU_PUBLIC(GAION) Styles {
public:
    static Styles *instance();

    int width() const;
    int height() const;

    QString title(QString const &) const;
    QString titleLink(QString const &link, QString const &) const;
    QString titleAddition(QString const &) const;
    QString titleTile(QString const &) const;
    QString titleTileLink(QString const &link, QString const &) const;
    QString subTitleTile(QString const &) const;

    QString job(QString const &) const;
    QString jobLink(QString const &link, QString const &) const;

    QString originalTitle(QString const &) const;
    QString originalTitleLabel(QString const &) const;

    QString tagline(QString const &) const;

    QString genresRuntime(QString const &) const;

    QString overview(QString const &) const;

    QString sectionHeader(QString const &) const;
    QString subHeader(QString const &) const;

    QString collection(QString const &link, QString const &) const;
    QString collectionLabel(QString const &) const;
    QString collectionNoLink(QString const &) const;

    QString tracks(QString const &) const;
    QString tracksLabel(QString const &) const;

    QString creditName(QString const &link, QString const &) const;
    QString creditDescription(QString const &) const;

    int bigImageWidth() const;

private:
    Styles();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion

#define GREU_STYL Styles::instance()
