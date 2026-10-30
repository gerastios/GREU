#include "Attribute.h"
#include <QObject>
#include "AttributeType.h"

using namespace std;

namespace greu::tag {

using namespace core;

struct Attribute::Detail {
    AttributeType const *const type;
    QValidator *validator = nullptr;
    Tag *tag;

    Detail(AttributeType const *type, Tag *tag)
        : type(type)
        , tag(tag) {}
};

Attribute::Attribute(AttributeType const *type, Tag *tag)
    : d(GREU_MKDET(type, tag)) {}

void Attribute::setTag(Tag *v) {
    d->tag = v;
}

Tag *Attribute::tag() const {
    return d->tag;
}

AttributeType const *Attribute::type() const {
    return d->type;
}

Attribute::ErrorType Attribute::copyValue(Attribute const &a) {
    if (type()->isReadOnly() || !type()->isCopyable())
        return ErrorType::NoError;
    else
        return copyValue_(a);
}

Attribute::ErrorType Attribute::copyValue_(Attribute const &) {
    return ErrorType::NoError;
}

void Attribute::setValidator(QValidator *value) {
    d->validator = value;
}

QValidator *Attribute::validator() const {
    return d->validator;
}

QString Attribute::errorText(ErrorType const &type, QString const &key) {
    switch (type) {
    case ErrorType::InvalidKey:
        return QObject::tr("%1: invalid key value '%2'").arg(d->type->longUpper(), key);
    case ErrorType::DuplicateKey:
        return QObject::tr("%1: duplicate key value '%2'").arg(d->type->longUpper(), key);
    case ErrorType::DescriptionEmpty:
        return QObject::tr("%1: description not filled in key '%2'").arg(d->type->longUpper(), key);
    case ErrorType::EntryNotFound:
        return QObject::tr("%1: entry with key '%2' not found").arg(d->type->longUpper(), key);
    default:
        return "";
    }
}

} // namespace greu::tag
