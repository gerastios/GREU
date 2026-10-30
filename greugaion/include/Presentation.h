#pragma once
#include "greucore/include/Uuid.h"

#include <any>

namespace greu::gaion {

enum GREU_PUBLIC(GAION) ListType : int {
    None,
    Features,
    Tiles,
    AllCredits,
};

struct GREU_PUBLIC(GAION) Presentation {
    struct GroupTab {
        QString title{};
        std::vector<core::Uuid> uuids;

        explicit GroupTab(core::Uuid const &);
        GroupTab(QString const &, core::Uuids const &);
        GroupTab(std::vector<core::Uuid> const &);

        bool operator==(GroupTab const &) const = default;
    };
    using GroupTabs = std::vector<GroupTab>;

    QString title{};
    ListType listType{None};
    GroupTabs groupTabs;
    std::any position;

    Presentation() = default;
    explicit Presentation(core::Uuid const &);
    explicit Presentation(core::Uuids const &);
    Presentation(QString const &, ListType, core::Uuids const &);
    Presentation(QString const &, ListType, std::vector<GroupTab> const &);
    GREU_COPY_DEF(Presentation);
    GREU_MOVE(Presentation);

    bool isEmpty() const;
    bool isSingle() const;
    bool hasUuid() const;
    core::Uuid singleUuid() const;
    size_t size() const;

    bool operator==(Presentation const &) const;
};

} // namespace greu::gaion
