#include "Credit.h"
#include <QString>

using namespace greu::core;

namespace greu::movies {
struct Credit::Detail {
    GREU_P_DET(tmdbId, QString)
    GREU_P_DET(personId, Uuid)
    GREU_P_DET(character, QString)
    GREU_P_DET(order, uint)
    GREU_P_DET(job, QString)
};

Credit::Credit(Uuid const &u)
    : Object(u)
    , d(GREU_MKDET()) {}

GREU_P_IMP(Credit, tmdbId, TmdbId, QString)
GREU_P_IMP(Credit, personId, PersonId, core::Uuid)
GREU_P_IMP(Credit, character, Character, QString)
GREU_P_IMP(Credit, order, Order, uint)
GREU_P_IMP(Credit, job, Job, QString)

} // namespace greu::movies
