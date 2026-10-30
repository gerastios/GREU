#pragma once
#include "LineEdit.h"

namespace greu::ui {

class GREU_PUBLIC(UI) HexEdit : public LineEdit {
public:
    HexEdit();

    void setBytes(QByteArray const &bytes);
    QByteArray bytes() const;

protected:
    virtual void contextMenuEvent(QContextMenuEvent *event) override;
};

} // namespace greu::ui
