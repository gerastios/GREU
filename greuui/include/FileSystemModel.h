#pragma once
#include <QFileSystemModel>
#include "greucore/include/DetailPtr.h"

namespace greu::ui {

class GREU_PUBLIC(UI) FileSystemModel : public QFileSystemModel {
public:
    using QFileSystemModel::QFileSystemModel;
    virtual void fetchMore(QModelIndex const &parent) override;
};

} // namespace greu::ui
