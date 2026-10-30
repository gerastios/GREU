#pragma once
#include "greucore/include/DetailPtr.h"

namespace greu::movies {

class GREU_PUBLIC(MOVIES) ObjectState {
public:
    ObjectState();
    virtual ~ObjectState() = default;

    void setNew(bool = true);
    bool isNew() const;
    void setDeleted(bool = true);
    bool isDeleted() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::movies
