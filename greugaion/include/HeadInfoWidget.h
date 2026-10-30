#pragma once
#include "greucore/include/DetailPtr.h"

class QVBoxLayout;
class QWidget;

namespace greu::gaion {

class PresentationWidget;
struct FeatureModel;

class GREU_PUBLIC(GAION) HeadInfoWidget {
public:
    HeadInfoWidget(PresentationWidget *, FeatureModel const *, bool addLink = false, QString const &typeString = {}, bool addSpecific = true);
    QWidget *widget() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion
