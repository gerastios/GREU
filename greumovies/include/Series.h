#pragma once
#include <QDate>
#include "Collectable.h"
#include "Credit.h"
#include "CreditHolder.h"
#include "Object.h"
#include "greucore/include/DetailPtr.h"
#include "property.h"
#include <set>

class QImage;

namespace greu::ui {
class ImageContainer;
}

namespace greu::movies {

class Season;

class GREU_PUBLIC(MOVIES) Series : public CreditHolder, public Collectable {
public:

    static Series *series(core::Uuid const &);

    static Series *forTmdb(int32_t);

    virtual void read() override;
    virtual void save() override;
    virtual void update() override;

    GREU_P_DEF(tmdbId, TmdbId, int32_t);
    GREU_P_DEF(lastAirDate, lastAirDate, QDate);
    GREU_P_DEF(originalName, OriginalName, QString);
    GREU_P_DEF(tagline, Tagline, QString);
    GREU_P_DEF(originalLanguage, OriginalLanguage, QString);
    GREU_P_DEF(originalLanguageCode, OriginalLanguageCode, QString);
    GREU_P_DEF(genres, Genres, std::set<QString>)
    virtual Credits const *castCredits() const override;
    virtual Credits const *crewCredits() const override;
    virtual Credits const *creatorCredits() const override;
    virtual Credits const *guestStarsCredits() const override;

    std::set<core::Uuid> const &seasonIds() const;
    std::vector<Season *> const seasons() const;

    virtual std::optional<QUrl> tmdbUrl() const override;
    virtual std::vector<tmdb::Credit> tmdbCreatorCredits() const override;
    virtual HolderType creditHolderType() const override { return CreditHolder::series; }
    virtual ObjectType objectType() const override { return ObjectType::series; }

    virtual QString nameInCollection() const override { return name(); }
    virtual Key key() const override { return {uuid(), Collectable::Type::series}; }

private:
    friend class Object;
    friend class SeriesTable;
    friend class SeasonsTable;
    Series(core::Uuid const &);
    void addSeasonId(core::Uuid const &);
    void setCreatorCredits(std::vector<tmdb::Credit> &&);
    void setCast(Credits  &&);
    void setCrew(Credits  &&);
    void setCreators(Credits  &&);
    Credit::Credits const &credits() const;
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual QImage emptyImage() const override;
};

} // namespace greu::movies
