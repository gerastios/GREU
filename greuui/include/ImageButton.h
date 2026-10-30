#pragma once
#include "ImageLabel.h"
#include "greucore/include/signalUtils.h"

class QImage;

namespace greu::ui {

class GREU_PUBLIC(UI) ImageButton: public ImageLabel {
public:
    ImageButton();
    void setImage(QImage const &, QString const &);
    core::StringSig &clicked() const;
private:
    struct Detail;
    core::DetailPtr<Detail> d;

};

}
