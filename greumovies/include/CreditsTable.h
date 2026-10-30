#pragma once
#include "Cleanable.h"
#include "Credit.h"
#include "CreditHolder.h"

namespace greu::movies {

class GREU_PUBLIC(MOVIES) CreditsTable : public Cleanable {
public:
    static CreditsTable *instance();
    virtual ~CreditsTable() = default;

    Credit::Credits read(CreditHolder const &);
    void save(Credit::Credits const &, CreditHolder const &);

    Credit::BundledCredits creditsOfPerson(core::Uuid const &) const;

    virtual uint64_t clean() override;

    static int to(CreditHolder::CreditType t);
    static int to(CreditHolder::HolderType t);
    static CreditHolder::CreditType to(int i);
    static CreditHolder::HolderType toHolderType(int i);

    static QString creatorJob();

private:
    CreditsTable();
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies
