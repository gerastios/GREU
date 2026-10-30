#pragma once

#include "greucore/include/DetailPtr.h"

class QString;
class QWidget;
class QTreeView;

namespace greu::ui {
class FileSystemModel;
} // namespace greu::ui

namespace greu::grylizo {

class DirectoryTree {
public:
    DirectoryTree();

    QWidget *setupUi(QString const &name);

    QTreeView *treeView() const;
    ui::FileSystemModel *model() const;

    void selectPath(QString const &path);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
