#include "CreditRequester.h"
#include <QJsonArray>
#include "TheMovieDatabase.h"
#include "greucore/include/jsonUtils.h"

using namespace std;
using namespace greu::tmdb;
using namespace greu::core;

namespace {
void addCast(vector<CastCredit> &credits, QJsonValue const &json, Credit::Type t) {
    auto o = json.toObject();
    credits.push_back({{valueString(o, "credit_id"), t, valueInt(o, "id")},
                       valueString(o, "character"),
                       valueInt(o, "order")});
}
} // namespace

namespace greu::tmdb {

using namespace core;

struct CreditRequester::Detail {
    QUrl url;
};

CreditRequester::CreditRequester(QUrl const &u)
    : TmdbRequester()
    , d(GREU_MKDET()) {
    d->url = u;
}

QUrl CreditRequester::url() const {
    return d->url;
}

CreditRequester::Data CreditRequester::read() {
    auto json = readTmdb();
    CreditRequester::Data res;
    for (auto c : json.value("cast").toArray())
        addCast(res.castCredits, c, Credit::cast);
    for (auto c : json.value("crew").toArray()) {
        auto o = c.toObject();
        res.crewCredits.push_back({{valueString(o, "credit_id"), Credit::crew, valueInt(o, "id")},
                                   valueString(o, "job"),
                                   valueString(o, "department")});
    }
    for (auto c : json.value("guest_stars").toArray())
        addCast(res.guestStarsCredits, c, Credit::guestStars);
    return res;
}

} // namespace greu::tmdb
