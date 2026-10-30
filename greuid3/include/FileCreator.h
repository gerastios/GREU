#pragma once
#include "greutag/include/FileCreator.h"

namespace greu::id3 {

class GREU_PUBLIC(ID3) FileCreator : public tag::FileCreator {
public:
    FileCreator();

    virtual bool handles(QFileInfo const &) const override;
    virtual std::unique_ptr<tag::File> createFile(QFileInfo const &) override;

};
}
