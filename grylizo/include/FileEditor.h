#pragma once

#include "greucore/include/DetailPtr.h"
#include <boost/signals2.hpp>

class QChar;
class QWidget;

namespace greu::core {
class Log;
}

namespace greu::tag {
class File;
}

namespace greu::grylizo {

class MainEditor;
class KeyedListAttributeEditor;
class AdvancedEditor;

class FileEditor {
public:
    FileEditor();

    GREU_NO_MOVE(FileEditor);
    GREU_NO_COPY(FileEditor);

    QWidget *setupUi();
    void commit();
    void update();

    void setFile(tag::File *);
    tag::File *file() const;

    MainEditor *mainEditor() const;
    KeyedListAttributeEditor *imagesEditor() const;
    AdvancedEditor *advancedEditor() const;

    bool addMnemonic(QChar const &);

    using FileSig = boost::signals2::signal<void(tag::File *)>;
    FileSig &fileSet() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
