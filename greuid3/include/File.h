#pragma once
#include "greutag/include/File.h"

class QFileInfo;

namespace greu::id3 {

class GREU_PUBLIC(ID3) File : public tag::File {
public:
    struct TagLocation {
        qint64 offset = 0;
        qint64 length = 0;
        int majorVersion = 0;
        int revision = 0;
        bool isV1() const { return majorVersion == 1; }
    };

    explicit File(QFileInfo const &);

    virtual tag::Tag *tag() const override;

private:
    virtual void analyzeFile(QFile *) override;
    virtual bool saveContent() override;

    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::id3
