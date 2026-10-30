#pragma once
#include <QLabel>
#include "greucore/include/DetailPtr.h"

class QImage;

namespace greu::ui {

class GREU_PUBLIC(UI) ImageLabel : public QLabel {
public:
    ImageLabel();
    void setImage(QImage const &, bool = true);
    QImage const &image() const;
    void showImage();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
