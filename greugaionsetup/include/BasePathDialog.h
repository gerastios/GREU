#pragma once
#include <QString>
#include "greucore/include/DetailPtr.h"

namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) BasePathDialog {
public:
    explicit BasePathDialog(QString const &current);
    bool execute();
    QString newPath() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion::setup
