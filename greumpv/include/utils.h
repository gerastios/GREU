#pragma once
#include "greucore/include/greuGlobal.h"
#include <mpv/client.h>

class QVariant;

namespace greu::mpv {

QVariant GREU_PUBLIC(MPV) toVariant(mpv_node const *node);

}
