#pragma once
#include "greucore/include/greuGlobal.h"

namespace greu::movies {

struct CreditHolder;
class Credit;
class Person;

struct GREU_PUBLIC(GAION) MediumCredit {
    QString job;
    Person *person;
};

struct GREU_PUBLIC(GAION) MediumDepartmentCredits {
    QString department;
    std::vector<MediumCredit> credits;
};
using MediumDepartmentsCredits = std::vector<MediumDepartmentCredits>;
using MediumCredits = std::vector<MediumCredit>;

QString GREU_PUBLIC(GAION) buildCrewJob(Credit const *);
QString GREU_PUBLIC(GAION) getCrewJob(Credit const *);
QString GREU_PUBLIC(GAION) buildCastJob(Credit const *);
QString GREU_PUBLIC(GAION) getCastJob(Credit const *);

MediumDepartmentsCredits GREU_PUBLIC(GAION) complete(CreditHolder const*);
MediumDepartmentsCredits GREU_PUBLIC(GAION) completeCast(CreditHolder const*);
MediumDepartmentsCredits GREU_PUBLIC(GAION) completeCrew(CreditHolder const*);
MediumCredits GREU_PUBLIC(GAION) importantCrew(CreditHolder const*, int);
MediumCredits GREU_PUBLIC(GAION) importantCast(CreditHolder const*, int);
MediumCredits GREU_PUBLIC(GAION) importantCrew(std::vector<CreditHolder const*> const &, int);
MediumCredits GREU_PUBLIC(GAION) importantCast(std::vector<CreditHolder const*> const &, int);
MediumCredits GREU_PUBLIC(GAION) creators(CreditHolder const*, int);
MediumCredits GREU_PUBLIC(GAION) guestStars(CreditHolder const*, int);
MediumCredits GREU_PUBLIC(GAION) crewForJob(CreditHolder const*, QString const &, int);

size_t GREU_PUBLIC(GAION) sizeOfCredits(MediumDepartmentsCredits const &);

}
