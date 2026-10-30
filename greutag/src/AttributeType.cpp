#include "AttributeType.h"
#include <QString>
#include "Attribute.h"
#include <map>

using namespace std;

namespace greu::tag {

using namespace core;

struct AttributeType::Detail {
    map<TextType, QString> texts;
    uint id;
    bool copyable = true, readOnly = false;

    Detail(StringsInitializer const &texts, bool copyable, bool readOnly)
        : copyable(copyable)
        , readOnly(readOnly) {
        int i = 0;
        for (auto &t : texts) {
            switch (i) {
            case 0:
                this->texts[TextType::ShortLower] = t;
                break;
            case 1:
                this->texts[TextType::ShortUpper] = t;
                break;
            case 2:
                this->texts[TextType::LongLower] = t;
                break;
            case 3:
                this->texts[TextType::LongUpper] = t;
                break;
            case 4:
                this->texts[TextType::PluralLower] = t;
                break;
            case 5:
                this->texts[TextType::PluralUpper] = t;
                break;
            case 6:
                this->texts[TextType::KeyLower] = t;
                break;
            case 7:
                this->texts[TextType::KeyUpper] = t;
                break;
            }
            i++;
        }
    }

    QString const &text(TextType const &type) const { return texts.at(type); }
};

AttributeType::AttributeType(StringsInitializer const &texts, bool copyable, bool readOnly)
    : d(GREU_MKDET(texts, copyable, readOnly)) {}

QString const &AttributeType::shortLower() const {
    return d->text(TextType::ShortLower);
}

QString const &AttributeType::shortUpper() const {
    return d->text(TextType::ShortUpper);
}

QString const &AttributeType::longLower() const {
    return d->text(TextType::LongLower);
}

QString const &AttributeType::longUpper() const {
    return d->text(TextType::LongUpper);
}

QString const &AttributeType::pluralLower() const {
    return d->text(TextType::PluralLower);
}

QString const &AttributeType::pluralUpper() const {
    return d->text(TextType::PluralUpper);
}

QString const &AttributeType::keyLower() const {
    return d->text(TextType::KeyLower);
}

QString const &AttributeType::keyUpper() const {
    return d->text(TextType::KeyUpper);
}

QString const &AttributeType::text(TextType const &type) const {
    return d->text(type);
}

void AttributeType::setReadOnly(bool value) {
    d->readOnly = value;
}

bool AttributeType::isReadOnly() const {
    return d->readOnly;
}

bool AttributeType::isCopyable() const {
    return d->copyable;
}

bool AttributeType::hasAttribute(Attribute *attribute) const {
    return attribute->type() == this;
}

void AttributeType::setId(uint value) {
    d->id = value;
}

uint AttributeType::id() const {
    return d->id;
}

} // namespace greu::tag
