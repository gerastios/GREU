#include "Cleanable.h"
#include "Object.h"
#include "greusql/include/Query.h"

using namespace std;

namespace greu::movies {

using namespace core;
using namespace sql;

UuidSet Cleanable::selectToDelete(sql::Query &q) {
    UuidSet res;
    q.execute();
    while (q.next())
        res.insert(q.value(0));
    return res;
}

uint64_t Cleanable::deleteRecords(Field const &field, UuidSet const &uuids) {
    for (auto const &u : uuids) {
        try {
            deleteEqual({u}, {&field});
        } catch (exception &e) {
            assert(false);
        }
        Object::remove(u);
    }
    return uuids.size();
}
} // namespace greu::movies
