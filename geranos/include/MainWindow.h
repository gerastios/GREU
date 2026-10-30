#pragma once

#include "greuui/include/MainWindow.h"

namespace greu::geranos {

class FileListTable;
class WorkFileTable;

class MainWindow : public ui::MainWindow {
public:
    MainWindow();

    FileListTable *fileList() const;
    WorkFileTable *fileTable() const;

private:
    virtual void setupUiInt() override;
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::geranos
