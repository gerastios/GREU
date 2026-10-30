#pragma once
#include "greucore/include/greuGlobal.h"

namespace greu::id3 {

class File;
class Tag;

class GREU_PUBLIC(ID3) Writer {
public:
    Writer() = default;
    GREU_NO_COPY(Writer);
    GREU_NO_MOVE(Writer);

    QByteArray write(Tag const &tag) const;

};

} // namespace greu::id3
