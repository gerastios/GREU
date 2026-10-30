#pragma once
#include "greucore/include/greuGlobal.h"
#include <functional>
#include <optional>

namespace greu::tag {

class File;

struct GREU_PUBLIC(TAG) FileHolder {

    virtual size_t size() const = 0;

    virtual File *at(size_t) = 0;
    virtual std::optional<size_t> indexOf(File *) = 0;

    using ReaderFunction = std::function<void(File const *)>;
    virtual void cforEach(ReaderFunction) const = 0;

    using WriterFunction = std::function<void(File *)>;
    virtual void forEach(WriterFunction) = 0;
};

} // namespace greu::tag
