#include "Tag.h"
#include "Attribute.h"
#include "AttributeType.h"
#include "FileNameValidator.h"
#include "greuui/include/LengthValidator.h"

using namespace std;
using namespace boost::signals2;
;

// clang-format off
#define SETATT_IMPL(NAME, UNAME, TYPE) \
    void Tag::setAttribute##UNAME(unique_ptr<AttributeTypes::TYPE> value) { \
        d->setAttribute(std::move(value), AttributeTypes::type(AttributeTypes::NAME()), this); \
    }

#define GETATT_IMPL(NAME, UNAME, TYPE) \
    AttributeTypes::TYPE *Tag::attribute##UNAME() const { \
        auto a = d->attribute(AttributeTypes::NAME()); \
        if (a) \
            return safeCast<AttributeTypes::TYPE>(a); \
        return nullptr; \
    }

#define GET_IMPL(NAME, UNAME, TYPE) \
    AttributeTypes::TYPE::ValueType Tag::NAME() const {\
        return std::any_cast<AttributeTypes::TYPE::ValueType>(attribute##UNAME()->genericValue()); \
    }

#define SET_IMPL(NAME, UNAME, TYPE) \
    void Tag::set##UNAME(AttributeTypes::TYPE::ValueType value) const { \
        attribute##UNAME()->setGenericValue(value); \
    }
// clang-format on

using namespace greu::core;
using namespace greu::ui;
using namespace greu::tag;

namespace {
struct ValidatorFactory {
    static QIntValidator beatsPerMinute, discNumber, length, playlistDelay, trackNumber;
    static LengthValidator isrc;
    static FileNameValidator fileName;

    static QValidator *forType(uint id) {
        if (id == AttributeTypes::fileName())
            return &fileName;
        if (id == AttributeTypes::beatsPerMinute())
            return &beatsPerMinute;
        if (id == AttributeTypes::discNumber())
            return &discNumber;
        if (id == AttributeTypes::isrc())
            return &isrc;
        if (id == AttributeTypes::length())
            return &length;
        if (id == AttributeTypes::playlistDelay())
            return &playlistDelay;
        if (id == AttributeTypes::trackNumber())
            return &trackNumber;
        return nullptr;
    }
};

QIntValidator ValidatorFactory::beatsPerMinute{0, numeric_limits<int>::max()};
QIntValidator ValidatorFactory::discNumber{0, numeric_limits<int>::max()};
LengthValidator ValidatorFactory::isrc{12, 12};
QIntValidator ValidatorFactory::length{0, numeric_limits<int>::max()};
QIntValidator ValidatorFactory::playlistDelay{0, numeric_limits<int>::max()};
QIntValidator ValidatorFactory::trackNumber{1, numeric_limits<int>::max()};
FileNameValidator ValidatorFactory::fileName;

} // namespace

using namespace boost::signals2;

namespace greu::tag {

struct Tag::Detail {
    File *const file;

    unordered_map<uint, AttributeItem> attributes;

    Detail(File *file)
        : file(file) {
        assert(file && "File must be valid for a tag!");
    }

    void setAttribute(unique_ptr<Attribute> &&attribute, AttributeType const *type, Tag *that) {
        assert(!attributes.contains(type->id()) && "Overwriting of attribute forbidden");
        assert(attribute && "Attribute must not be empty");
        assert(type->hasAttribute(attribute.get()) && "Attribute has wrong attribute type");
        that->add(attribute.get());
        if (!attribute->validator())
            attribute->setValidator(ValidatorFactory::forType(type->id()));
        ScopedConnection conn = attribute->modifiedChanged().connect(
            [that](auto c) { that->modifiedChanged()(c); });
        attributes[type->id()] = {.attribute = std::move(attribute), .connection = std::move(conn)};
    }

    Attribute *attribute(uint id) const {
        if (contains(id))
            return attributes.at(id).attribute.get();
        return nullptr;
    }

    bool contains(int id) const { return attributes.contains(id); }

    void save() {
        for (auto a = attributes.begin(); a != attributes.end(); a++) {
            a->second.attribute->save();
        }
    }
};

Tag::Tag(File *file)
    : d(GREU_MKDET(file)) {}

File *Tag::file() const {
    return d->file;
}

bool Tag::contains(uint id) const {
    return d->contains(id);
}

Attribute *Tag::attribute(uint id) const {
    return d->attribute(id);
}

void Tag::setAttribute(std::unique_ptr<Attribute> &&value, AttributeType const *type) {
    d->setAttribute(std::move(value), type, this);
}

QString Tag::completeFileName() const {
    return fileName() + "." + fileExtension();
}

void Tag::clear() {
    d->attributes.clear();
    ModifiableCollection::clear();
}

void Tag::merge(Tag const &other) {
    for (auto &a : other.d->attributes) {
        auto t = a.second.attribute->type();
        auto v = a.second.attribute->genericValue();
        if (!contains(a.first)) {
            a.second.attribute->setTag(this);
            setAttribute(std::move(a.second.attribute), t);
        }
        auto att = attribute(t->id());
        if (att->isEmpty())
            att->setGenericValue(v);
    }
}

unordered_map<uint, Tag::AttributeItem> const &Tag::attributes() const {
    return d->attributes;
}

GREU_GRYLIZO_META_ATT(SETATT_IMPL);
GREU_GRYLIZO_META_ATT(GETATT_IMPL);
GREU_GRYLIZO_META_ATT(SET_IMPL);
GREU_GRYLIZO_META_ATT(GET_IMPL);

} // namespace greu::tag
