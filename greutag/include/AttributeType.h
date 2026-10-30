#pragma once

#include "greucore/include/DetailPtr.h"
#include <initializer_list>

namespace greu::tag {

class Attribute;

class GREU_PUBLIC(TAG) AttributeType {
public:
    enum TextType {
        ShortLower,
        ShortUpper,
        LongLower,
        LongUpper,
        PluralLower,
        PluralUpper,
        KeyLower,
        KeyUpper,
    };

    using StringsInitializer = std::initializer_list<QString const>;

    explicit AttributeType(StringsInitializer const &texts,
                           bool copyable = true,
                           bool readOnly = false);

    GREU_NO_COPY(AttributeType);
    GREU_NO_MOVE(AttributeType);

    QString const &shortLower() const;
    QString const &shortUpper() const;
    QString const &longLower() const;
    QString const &longUpper() const;
    QString const &pluralLower() const;
    QString const &pluralUpper() const;
    QString const &keyLower() const;
    QString const &keyUpper() const;

    QString const &text(TextType const &) const;

    void setReadOnly(bool);
    bool isReadOnly() const;
    bool isCopyable() const;
    bool hasAttribute(Attribute *) const;

    void setId(uint);
    uint id() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tag
