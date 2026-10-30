#pragma once
#include "greutag/include/Tag.h"

class QFile;
class QFileInfo;

namespace greu::core {
class Log;
class StaticSelector;
}

namespace greu::id3 {

class File;
class Frame;

class GREU_PUBLIC(ID3) Tag : public tag::Tag {
public:
    explicit Tag(File *);
    virtual ~Tag() = default;

    static std::unordered_map<QString, QString> const &genres();
    static QString genreName(QString);

    static core::StaticSelector &eventTimingCodesUnitsSelector();
    static core::StaticSelector &eventTimingCodesTypesSelector();

    std::vector<Frame> const &additionalFrames() const;
    void addAdditionalFrame(Frame const &);
    void clearAdditionalFrames();

    virtual void clear() override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

} // namespace greu::id3
