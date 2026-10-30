#pragma once
#include "greucore/include/DetailPtr.h"

class QImage;

namespace greu::ui {

class GREU_PUBLIC(CORE) ImageContainer {
public:
    enum Type : int {
        Unknown,
        WindowsBitmap,
        GraphicInterchangeFormat,
        JointPhotographicExpertsGroup,
        PortableNetworkGraphics,
        PortableBitmap,
        PortableGraymap,
        PortablePixmap,
        X11Bitmap,
        X11Pixmap,
        ScalableVectorGraphics,
        TaggedImageFileFormat,
        WebP,
    };

    ImageContainer();
    explicit ImageContainer(QByteArray const &, Type);
    ImageContainer(ImageContainer const &);
    ImageContainer &operator=(ImageContainer const &);
    static ImageContainer fromImage(QImage const &);

    GREU_MOVE(ImageContainer)

    void setImage(QByteArray const &, Type);
    bool isValid() const;
    bool isEmpty() const;
    Type type() const;
    QByteArray const &data() const;

    QImage toImage() const;

    static Type fromExension(QString const &);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::ui
