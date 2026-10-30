#pragma once
#include "greucore/include/DetailPtr.h"

namespace greu::core {
class Uuid;
}
namespace greu::gaion {

struct Presentation;

class GREU_PUBLIC(GAION) LinkController {
public:
    LinkController();

    QString buildLink(Presentation const);
    Presentation presentation(QString const &) const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion
