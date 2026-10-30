#pragma once
#include <QDate>
#include "Collectable.h"
#include "Credit.h"
#include "CreditHolder.h"
#include "Object.h"
#include "OwnMovie.h"
#include "Video.h"
#include "greucore/include/DetailPtr.h"
#include "property.h"
#include <set>

namespace greu::movies {

class File;

class GREU_PUBLIC(MOVIES) Movie : public Video, public Collectable {
public:
    static Movie *movie(core::Uuid const &);
    static Movie *createFromTmdb(int32_t, QString const & = "");
    static Movie *createFromOwnMovie(OwnMovie const &);

    virtual void read() override;
    virtual void save() override;
    virtual void update() override {};

    GREU_P_DEF(tmdbId, TmdbId, int32_t);
    GREU_P_DEF(tagline, Tagline, QString);
    virtual QString const &name(bool = false) const override;
    GREU_P_DEF(variantTitle, VariantTitle, QString);
    GREU_P_DEF(originalLanguage, OriginalLanguage, QString);
    GREU_P_DEF(originalLanguageCode, OriginalLanguageCode, QString);
    GREU_P_DEF(originalTitle, OriginalTitle, QString);
    GREU_P_DEF(genres, Genres, std::set<QString>);
    virtual Credits const *castCredits() const override;
    virtual Credits const *crewCredits() const override;
    virtual Credits const *creatorCredits() const override;
    virtual Credits const *guestStarsCredits() const override;

    virtual std::optional<QUrl> tmdbUrl() const override;
    virtual HolderType creditHolderType() const override { return CreditHolder::movie; }
    virtual ObjectType objectType() const override { return ObjectType::movie; }

    virtual QString nameInCollection() const override { return name(); }
    inline virtual Key key() const override { return {uuid(), Collectable::Type::movie}; }

    std::optional<File *> file() const;

private:
    friend class Object;
    friend class MoviesTable;
    void setCast(Credits  &&);
    void setCrew(Credits  &&);
    Credit::Credits const &credits() const;
    explicit Movie(core::Uuid const &);
    struct Detail;
    core::DetailPtr<Detail> d;

    virtual QImage emptyImage() const override;
};

} // namespace greu::movies
