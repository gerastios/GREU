#include "FileSystemModel.h"

namespace greu::ui {

using namespace core;

void FileSystemModel::fetchMore(QModelIndex const &parent) {
    QFileSystemModel::fetchMore(parent);
    auto root = rootPath();
    setRootPath(fileName(parent));
    sort(0);
    setRootPath(root);
}

} // namespace greu::ui
