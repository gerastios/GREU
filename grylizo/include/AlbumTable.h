#pragma once

#include "greucore/include/DetailPtr.h"
#include <boost/signals2.hpp>

class QWidget;

namespace greu::ui {
class TableView;
}

namespace greu::tag {
class AudioCollection;
class File;
class Album;
} // namespace greu::tag

namespace greu::grylizo {

class MainWindow;

class AlbumTable {
public:
    AlbumTable();

    QWidget *setupUi();
    ui::TableView *table() const;

    void setActiveAlbum(tag::Album const *album);
    void update();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
