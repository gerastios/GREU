#pragma once
#include <QFileInfo>
#include "greucore/include/signalUtils.h"
#include "greugaion/include/MainWindow.h"

class QTreeView;

namespace greu::ui {
class FileSystemModel;
} // namespace greu::ui

namespace greu::movies {
class File;
}

namespace greu::gaion::setup {

class GREU_PUBLIC(GAIONSETUP) MainWindow : public gaion::MainWindow {
public:
    MainWindow();
    std::vector<QFileInfo> selectedFiles() const;

    QTreeView *fileTree() const;
    ui::FileSystemModel *fileModel() const;

    virtual void setMediumWidget(PresentationWidget *) override;
    virtual PresentationWidget *mediumWidget() const override;

    core::FileInfoSig &assignMovieRequested() const;
    core::FileInfoSig &unassignFileRequested() const;
    core::FileInfoSig &updateFileRequested() const;
    core::FileInfoSig &assignEpisdoeRequested() const;
    core::FileInfoSig &assignOwnMovieRequested() const;
    core::FileInfoSig &playFileRequested() const;
    core::FileInfoSig &fileSelected() const;

private:
    struct Detail;
    core::DetailPtr<Detail> d;
    virtual void setupUiInt() override;

};
} // namespace greu::gaion::setup
