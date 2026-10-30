#pragma once
#include <QObject>
#include <QString>
#include "greucore/include/Selector.h"

namespace greu::id3 {

class GREU_PUBLIC(ID3) UrlTypeSelector : public core::Selector {
public:
    static UrlTypeSelector *instance();

    struct TypeDescription {
        uint type;
        QString description;
        bool onlyOneValue;
        bool descriptionDependent;
    };

    using ConstTypeIterator = std::unordered_map<QString, TypeDescription>::const_iterator;

    uint typeOfId(QString const &id) const;
    QString descriptionOfType(uint) const;
    QString idOfType(uint) const;

    ConstTypeIterator begin() const;
    ConstTypeIterator end() const;

private:
    UrlTypeSelector();
};

} // namespace greu::id3
