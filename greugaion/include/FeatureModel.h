#pragma once
#include <QDateTime>
#include "greucore/include/Uuid.h"

namespace greu::movies {
class File;
};

namespace greu::gaion {

class PresentationItem;

struct GREU_PUBLIC(GAION) FeatureModel {
    virtual ~FeatureModel() = default;

    virtual QString title() const = 0;
    virtual QString titleLabel() const = 0;
    virtual QImage image() const = 0;
    virtual std::optional<core::Uuid> linkedUuid() const { return std::nullopt; }
    virtual QString originalTitle() const { return {}; }
    virtual QString tagline() const { return {}; }
    virtual QString description() const { return {}; }
    virtual QString descriptionLabel() const { return {}; }
    virtual movies::File *file() const { return nullptr; }
    virtual QDate born() const { return {}; }
    virtual QString bornPlace() const { return {}; }
    virtual QDate died() const { return {}; }
    virtual int age() const { return 0; }
    virtual QString knownForDepartment() const { return {}; }

    virtual QDate releaseDate() const { return {}; }
    virtual std::set<QString> genres() const { return {}; }
    struct BelongEntry {
        QString title;
        core::Uuid uuid;
    };
    struct BelongGroup {
        QString type;
        std::vector<BelongEntry> entries;
    };
    using BelongGroups = std::vector<BelongGroup>;
    virtual inline BelongGroups belongsTo() const { return {}; }

};

} // namespace greu::gaion
