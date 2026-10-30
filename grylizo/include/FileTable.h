#pragma once

#include "greucore/include/DetailPtr.h"
#include <boost/signals2.hpp>

class QModelIndex;
class QWidget;

namespace greu::tag {
class FileContainer;
class File;
} // namespace greu::tag
namespace greu::ui {
class TableView;
class DispatchedUiComponent;
} // namespace greu::ui
namespace greu::grylizo {

class Dispatcher;

class FileTable {
public:
    enum DisplayType : int {
        None,
        Directory,
        Album,
    };

    FileTable();

    QWidget *setupUi();

    void setFileContainer(tag::FileContainer *, bool = false);
    tag::FileContainer *fileContainer() const;
    void selectFile(tag::File *, bool emitSignal);
    DisplayType displayType() const;
    std::vector<tag::File *> selectedFiles() const;
    ui::TableView *table() const;
    tag::File *fileAt(size_t index) const;
    QModelIndex index(int row, int column) const;

    using ContainerSig = boost::signals2::signal<void(tag::FileContainer *)>;
    ContainerSig &containerSet() const;

    void sortByHeaderInfo();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
