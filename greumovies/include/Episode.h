#pragma once
#include <QDate>
#include "Collectable.h"
#include "Credit.h"
#include "CreditHolder.h"
#include "Object.h"
#include "Video.h"
#include "greucore/include/DetailPtr.h"
#include "property.h"

namespace greu::movies {

class Series;
class Season;
class File;

class GREU_PUBLIC(MOVIES) Episode : public Video, public Collectable {
public:
    struct TmdbKey {
        int32_t seriesId, seasonNumber, episodeNumber;
    };

    static Episode *episode(core::Uuid const &);
    static Episode *createFromTmdb(TmdbKey const &,
                                   int32_t variantSeasonNumber = 0,
                                   int32_t variantEpisodeNumber = 0,
                                   QString const & = {});

    virtual void read() override;
    virtual void save() override;
    virtual void update() override {};

    GREU_P_DEF(tmdbId, TmdbId, TmdbKey);
    int32_t number(bool = false) const;
    int32_t seasonNumber(bool = false) const;
    GREU_P_DEF(variantSeasonNumber, VariantSeasonNumber, int32_t);
    GREU_P_DEF(variantEpisodeNumber, VariantEpisodeNumber, int32_t);
    GREU_P_DEF(originalName, OriginalName, QString);
    virtual Credits const *creatorCredits() const override;
    virtual Credits const *castCredits() const override;
    virtual Credits const *crewCredits() const override;
    virtual Credits const *guestStarsCredits() const override;

    core::Uuid seasonId() const;
    Season *season() const;
    Series *series() const;

    virtual std::optional<QUrl> tmdbUrl() const override;
    virtual HolderType creditHolderType() const override { return CreditHolder::episode; }
    virtual ObjectType objectType() const override { return ObjectType::episode; }

    virtual Key key() const override { return {uuid(), Collectable::Type::episode}; }
    virtual QString nameInCollection() const override;

    std::optional<File *> file() const;

private:
    friend class Object;
    friend class EpisodesTable;
    void setCast(Credits &&);
    void setCrew(Credits &&);
    void setGuestStars(Credits &&);
    void setSeasonId(core::Uuid const &);
    Credit::Credits const &credits() const;
    explicit Episode(core::Uuid const &);
    struct Detail;
    core::DetailPtr<Detail> d;
    virtual QImage emptyImage() const override;
};

} // namespace greu::movies
