#pragma once
#include <QValidator>
#include "greucore/include/DetailPtr.h"

namespace greu::tag {

class File;

class GREU_PUBLIC(TAG) FileNameValidator : public QValidator {
public:
    FileNameValidator();

    void setFile(File *);
    virtual State validate(QString &text, int &) const override;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::tag
