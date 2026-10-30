#pragma once
#include "FileContainer.h"
#include "greucore/include/DetailPtr.h"

namespace greu::tag {

class GREU_PUBLIC(TAG) Directory : public FileContainer {
public:
    explicit Directory(QString const &path);
    virtual ~Directory() = default;

    QString const path() const;

    bool isModified() const;

    virtual void addFile(File *) override;
    virtual void removeFile(File *) override;
    virtual size_t size() const override;
    virtual File *at(size_t index) override;
    virtual std::optional<size_t> indexOf(File *) override;
    virtual void sort(uint typeId, Qt::SortOrder) const override;
    virtual QString const caption() const override;
    virtual QIcon const tagIcon() const override;
    virtual QString const tagText() const override;
    virtual FileContainerSig &fileAdded() const override;
    virtual FileContainerSig &fileRemoved() const override;
    virtual Iterator begin() override;
    virtual Iterator end() override;
    virtual ConstIterator cbegin() override;
    virtual ConstIterator cend() override;
    virtual void cforEach(ReaderFunction) const override;
    virtual void forEach(WriterFunction) override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};
} // namespace greu::tag
