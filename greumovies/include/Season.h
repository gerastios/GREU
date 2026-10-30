#pragma once
#include <QDate>
#include "Credit.h"
#include "CreditHolder.h"
#include "Object.h"
#include "greucore/include/DetailPtr.h"
#include "property.h"
#include <set>

class QDate;
class QImage;

namespace greu::ui {
class ImageContainer;
}

namespace greu::movies {

class Series;
class Episode;

class GREU_PUBLIC(MOVIES) Season : public CreditHolder {
public:
    static Season *season(core::Uuid const &);

    static Season *forTmdb(int32_t seriesId, int32_t, QString const &originalLanguageCode1);

    virtual void read() override;
    virtual void save() override;
    virtual void update() override;

    int32_t number() const;
    core::Uuid seriesId() const;
    Series *series() const;

    using IntPair = std::pair<int32_t, int32_t>;

    GREU_P_DEF(tmdbId, TmdbId, IntPair);
    GREU_P_DEF(originalName, OriginalName, QString);
    virtual Credits const *castCredits() const override;
    virtual Credits const *crewCredits() const override;
    virtual Credits const *creatorCredits() const override;
    virtual Credits const *guestStarsCredits() const override;

    std::set<core::Uuid> const &episodeIds() const;
    std::vector<Episode *> episodes() const;

    virtual std::optional<QUrl> tmdbUrl() const override;
    virtual HolderType creditHolderType() const override { return CreditHolder::season; }
    virtual ObjectType objectType() const override { return ObjectType::season; }

private:
    friend class Object;
    friend class SeasonsTable;
    friend class Episode;
    Season(core::Uuid const &);
    void addEpisodeId(core::Uuid const &);
    void setCast(Credits &&);
    void setCrew(Credits &&);
    Credit::Credits const &credits() const;
    void setSeriesId(core::Uuid const &);
    void setNumber(int32_t);
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual QImage emptyImage() const override;
};

} // namespace greu::movies
