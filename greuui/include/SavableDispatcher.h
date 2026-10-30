#pragma once
#include "Dispatcher.h"
#include "greucore/include/Modifiable.h"

namespace greu::core { class Saver; }

namespace greu::ui {

class GREU_PUBLIC(UI) SavableDispatcher: public Dispatcher, public core::Modifiable {
public:
    SavableDispatcher();

    virtual size_t modifiedCount() const = 0;
    virtual size_t savedCount() const = 0;
    void saveModified(bool noCancel = false);
    void saveAll(bool noCancel = false);
    virtual bool canClose() override;
    virtual bool noUiUpdate() const override;

private:
    virtual void updateUi() = 0;
    virtual core::Saver *saver() const = 0;

    struct Detail;
    core::DetailPtr<Detail> d;

};

}
