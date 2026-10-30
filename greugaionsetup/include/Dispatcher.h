#pragma once
#include "greugaion/include/Dispatcher.h"

class QFileInfo;

namespace greu::gaion::setup {

class Actions;

class GREU_PUBLIC(GAIONSETUP) Dispatcher : public gaion::Dispatcher {
public:
    Dispatcher();

    virtual void initialize() override;
    virtual void shutDown() override;
    virtual ui::MainWindow *mainWindow() const override;
    inline virtual bool canClose() override { return true; }

    void removeUnused();
    void deleteCollection();

    void addMediumToCollection();
    void removeMediumFromCollection();
    void setImageForObject();
    void updateObject();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion::setup
