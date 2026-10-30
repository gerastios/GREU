#pragma once
#include "greucore/include/DetailPtr.h"

class QWidget;

namespace greu::gaion {

class PresentationWidget;
struct FeatureModel;

class GREU_PUBLIC(GAION) HeadWidget {
public:
    HeadWidget(PresentationWidget *, FeatureModel const *, bool addLink = false, QString const &typeString = {}, bool addSecific = true);
    QWidget *widget() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion
