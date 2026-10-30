#include "ImageReader.h"
#include <QFileInfo>
#include "Reader.h"
#include "TheMovieDatabase.h"
#include "greuui/include/ImageContainer.h"

namespace greu::tmdb {

using namespace core;
using namespace ui;

struct ImageReader::Detail {
    QString const fileName;
    bool thumbnail;
    ImageContainer image;

    Detail(QString const &fileName, bool thumbnail)
        : fileName(fileName)
        , thumbnail(thumbnail) {}
};

ImageReader::ImageReader(QString const &fileName, bool thumbnail)
    : d(GREU_MKDET(fileName, thumbnail)) {}

QUrl ImageReader::url() const {
    return QUrl{GREU_TMDB->imagePath()
                + (d->thumbnail ? GREU_TMDB->imageSmallSizeInfix() : GREU_TMDB->imageBigSizeInfix())
                + d->fileName};
}

void ImageReader::finished(QNetworkReply *reply) {
    d->image.setImage(reply->readAll(),
                      ImageContainer::fromExension(QFileInfo{d->fileName}.suffix()));
}

ImageContainer ImageReader::read() {
    Reader{}.read(this);
    return d->image;
}

} // namespace greu::tmdb
