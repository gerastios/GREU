#pragma once

#include "greuui/include/Dialog.h"

namespace greu::geranos {

class ChapterEditor : public ui::Dialog {
public:
    ChapterEditor(QString const &filePath);
    ~ChapterEditor();

    QString chapters() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::geranos
