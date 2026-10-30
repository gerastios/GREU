#pragma once
#include "DateTimeAttribute.h"
#include "ImagesAttribute.h"
#include "ListAttribute.h"
#include "ValuedKeyedListAttribute.h"
#include "attributeTypeMeta.h"

#define GETID_DECL(NAME, UNAME, TYPE) static uint NAME();

namespace greu::tag {

class GREU_PUBLIC(TAG) AttributeTypes {
public:
    using PlainStringAttribute = PlainAttribute<QString>;
    using PlainUintAttribute = PlainAttribute<uint>;
    using PlainTimeAttribute = DateTimeAttribute;
    using PlainBytesAttribute = PlainAttribute<QByteArray>;

    using ListStringAttribute = ListAttribute<QString>;

    using StringKeyedListBytesAttribute = KeyedListAttribute<QString, QByteArray>;
    using StringKeyedListStringAttribute = KeyedListAttribute<QString, QString>;
    using UintKeyedListBytesAttribute = KeyedListAttribute<uint, QByteArray>;
    using UintKeyedListStringAttribute = KeyedListAttribute<uint, QString>;
    using UintKeyedListUintAttribute = KeyedListAttribute<uint, uint>;
    using UintKeyedListImageAttribute = ImagesAttribute;

    using UintValuedUintKeyedListUintAttribute = ValuedKeyedListAttribute<uint, uint, uint>;

    static uint registerAttributeType(std::unique_ptr<AttributeType> value);
    static AttributeType *type(uint id);

public:
    inline static uint null() { return 0; }

    GREU_GRYLIZO_META_ATT(GETID_DECL)
};

} // namespace greu::tag
#undef GETID_DECL
