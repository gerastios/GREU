#include "ImageImporter.h"
#include <QApplication>
#include <QClipboard>
#include <QImage>
#include <QMimeData>
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/ImageContainer.h"
#include "greuui/include/ImageTool.h"

using namespace greu::core;
using namespace greu::ui;

namespace greu::gaion::setup {

ImageContainer ImageImporter::import() {
    auto cl = QApplication::clipboard();
    cl->ownsClipboard();
    if (!cl->mimeData()->hasImage())
        return {};
    auto image = cl->image();
    if (image.isNull())
        return {};
    GuardedPtr<ImageTool> tool;
    tool = new ImageTool(image, QObject::tr("Person poster"), 0);
    if (tool->exec()) {
        image = tool->image();
        return ImageContainer::fromImage(image);
    }
    return {};
}

} // namespace greu::gaion::setup
