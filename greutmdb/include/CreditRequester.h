#pragma once
#include "Credit.h"
#include "TmdbRequester.h"

namespace greu::tmdb {

class GREU_PUBLIC(TMDB) CreditRequester : public TmdbRequester {
public:
    struct Data {
        std::vector<CastCredit> castCredits;
        std::vector<CrewCredit> crewCredits;
        std::vector<CastCredit> guestStarsCredits;
    };

    CreditRequester(QUrl const &);

    virtual QUrl url() const override;
    Data read();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tmdb
