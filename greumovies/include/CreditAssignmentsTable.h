#pragma once
#include "Cleanable.h"
#include "CreditHolder.h"

namespace greu::core {
class Uuid;
}

namespace greu::movies {

class GREU_PUBLIC(MOVIES) CreditAssignmentTable : public Cleanable {
public:
    static CreditAssignmentTable *instance();

    void save(core::Uuid const &, CreditHolder const &, CreditHolder::CreditType);
    std::vector<std::pair<core::Uuid, CreditHolder::CreditType>> read(core::Uuid const &);
    virtual uint64_t clean() override;

private:
    CreditAssignmentTable();
    uint64_t deleteType(QString const &, int);
};

} // namespace greu::movies
