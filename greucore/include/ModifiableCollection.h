#pragma once

#include "Modifiable.h"

namespace greu::core {

class GREU_PUBLIC(CORE) ModifiableCollection: public Modifiable
{
public:
    ModifiableCollection();

    void add(Modifiable *);
    void remove(Modifiable *);
    void clear();

    virtual void setModified(bool) override;
    virtual bool isModified() const override;
    virtual bool save() override;

private:
    struct Detail;
    DetailPtr<Detail> d;

};
}
