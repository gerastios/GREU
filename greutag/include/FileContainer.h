#pragma once
#include <Qt>
#include "FileHolder.h"
#include <boost/signals2.hpp>
#include <vector>

class QIcon;

namespace greu::tag {

struct GREU_PUBLIC(TAG) FileContainer : public FileHolder {

    virtual void addFile(File *) = 0;
    virtual void removeFile(File *) = 0;

    virtual QString const caption() const = 0;
    virtual QIcon const tagIcon() const = 0;
    virtual QString const tagText() const = 0;

    using Iterator = std::vector<File *>::iterator;
    virtual Iterator begin() = 0;
    virtual Iterator end() = 0;

    using ConstIterator = std::vector<File *>::const_iterator;
    virtual ConstIterator cbegin() = 0;
    virtual ConstIterator cend() = 0;

    using FileContainerSig = boost::signals2::signal<void(FileContainer *, File *)>;
    virtual FileContainerSig &fileAdded() const = 0;
    virtual FileContainerSig &fileRemoved() const = 0;

    virtual void sort(uint typeId, Qt::SortOrder) const = 0;
};

} // namespace greu::tag
