#pragma once

#include "LongRunnable.h"

namespace greu::core {

class GREU_PUBLIC(CORE) Saver: public LongRunnable, public LongRunnable::Worker {
public:
    Saver();
    virtual ~Saver() = default;

    void setSaveAll(bool);
    bool isSaveAll() const;
    virtual void work(std::promise<void> * const) override;
    virtual std::__exception_ptr::exception_ptr exception() override;
    virtual void save() = 0;
    virtual void saveAll() = 0;
    inline virtual bool noDialog() { return false; }

private:
    struct Detail;
    DetailPtr<Detail> d;

};

}
