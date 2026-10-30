#pragma once
#include "AttributeTypes.h" // IWYU pragma:keep
#include "attributeTypeMeta.h"
#include "greucore/include/DetailPtr.h"
#include "greucore/include/ModifiableCollection.h"

#define GETATT_DECL(NAME, UNAME, TYPE) AttributeTypes::TYPE *attribute##UNAME() const;
#define SETATT_DECL(NAME, UNAME, TYPE) \
    void setAttribute##UNAME(std::unique_ptr<AttributeTypes::TYPE> value);
#define GET_DECL(NAME, UNAME, TYPE) AttributeTypes::TYPE::ValueType NAME() const;
#define SET_DECL(NAME, UNAME, TYPE) void set##UNAME(AttributeTypes::TYPE::ValueType value) const;

namespace greu::tag {

class File;
class Attribute;
class AttributeType;

class GREU_PUBLIC(TAG) Tag : public core::ModifiableCollection {
public:
    explicit Tag(File *);

    struct AttributeItem {
        std::unique_ptr<Attribute> attribute{};
        core::ScopedConnection connection{};
    };

    File *file() const;

    bool contains(uint id) const;
    Attribute *attribute(uint id) const;
    void setAttribute(std::unique_ptr<Attribute> &&value, AttributeType const *);

    GREU_GRYLIZO_META_ATT(GETATT_DECL);
    GREU_GRYLIZO_META_ATT(SETATT_DECL);
    GREU_GRYLIZO_META_ATT(GET_DECL);
    GREU_GRYLIZO_META_ATT(SET_DECL);

    QString completeFileName() const;

    virtual void clear();
    virtual void merge(Tag const &);

    std::unordered_map<uint, AttributeItem> const &attributes() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tag

#undef GETATT_DECL
#undef SETATT_DECL
#undef GET_DECL
#undef SET_DECL
