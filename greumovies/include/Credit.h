#pragma once
#include "CreditHolder.h"
#include "greucore/include/Uuid.h"
#include "property.h"
#include <vector>

namespace greu::movies {

class GREU_PUBLIC(MOVIES) Credit : public Object {
public:
    struct Credits {
        CreditHolder::Credits castCredits;
        CreditHolder::Credits crewCredits;
        CreditHolder::Credits creatorsCredits;
        CreditHolder::Credits guestStarsCredits;
    };

    struct BundledCredit {
        CreditHolder *holder;
        CreditHolder::CreditType type;
        Credit const *credit;
    };

    struct BundledCredits {
        std::vector<BundledCredit> castCredits, crewCredits;
    };

    virtual void read() override {};
    virtual void save() override {};

    GREU_P_DEF(tmdbId, TmdbId, QString)
    GREU_P_DEF(personId, PersonId, core::Uuid)
    GREU_P_DEF(character, Character, QString)
    GREU_P_DEF(order, Order, uint)
    GREU_P_DEF(job, Job, QString)

private:
    Credit(core::Uuid const &);
    friend class Object;
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies
