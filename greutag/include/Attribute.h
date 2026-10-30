#pragma once
#include "greucore/include//DetailPtr.h"
#include "greucore/include/Restorable.h"

#include <any>

class QValidator;

namespace greu::tag {

class AttributeType;
class Tag;

class GREU_PUBLIC(TAG) Attribute: public core::Restorable {
public:
    enum ErrorType {
        NoError,
        DuplicateKey,
        InvalidKey,
        DescriptionEmpty,
        EntryNotFound,
    };

    Attribute(AttributeType const *, Tag *);
    virtual ~Attribute() = default;

    GREU_NO_COPY(Attribute);
    GREU_NO_MOVE(Attribute);

    void setTag(Tag *);
    Tag * tag() const;
    AttributeType const * type() const;

    ErrorType copyValue(Attribute const &);

    void setValidator(QValidator *);
    QValidator *validator() const;

    QString errorText(ErrorType const & type, QString const & key = "");

    virtual void clear() = 0;
    virtual bool isEmpty() const = 0;
    virtual void setGenericValue(std::any const &) = 0;
    virtual std::any genericValue() = 0;

private:
    virtual ErrorType copyValue_(Attribute const &);
    struct Detail;
    core::DetailPtr<Detail> d;

};

}
