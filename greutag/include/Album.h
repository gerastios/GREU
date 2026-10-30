#pragma once
#include "FileContainer.h"
#include "greucore/include/DetailPtr.h"
#include <boost/container_hash/hash.hpp>

class QImage;

namespace greu::tag {

class Tag;

class GREU_PUBLIC(TAG) Album : public FileContainer {
public:
    struct Key {
        QString title;
        QString artist;
    };
    static Key buildKey(Tag const *);

    Album(Key const &);
    virtual ~Album() = default;

    QString const artist() const;
    QString const title() const;
    QString const artistSort() const;
    QString const titleSort() const;
    Key const key() const;
    QImage const image() const;
    size_t trackCount() const;
    bool isIncomplete() const;

    void determineImage();

    virtual void addFile(File *) override;
    virtual void removeFile(File *) override;
    virtual size_t size() const override;
    virtual File *at(size_t) override;
    virtual void sort(uint typeId, Qt::SortOrder) const override;
    virtual std::optional<size_t> indexOf(File *) override;
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

inline bool operator==(Album::Key const &left, Album::Key const &right) {
    return left.artist == right.artist && left.title == right.title;
}

} // namespace greu::tag

namespace std {
template<>
struct hash<greu::tag::Album::Key> {
    std::size_t operator()(greu::tag::Album::Key const &t) const {
        size_t seed = 0;
        boost::hash_combine(seed, t.title.toStdWString());
        boost::hash_combine(seed, t.artist.toStdWString());
        return seed;
    }
};

} // namespace std
