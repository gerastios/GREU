#pragma once

#include "greuui/include/MainWindow.h"

namespace greu::ui {
class LabelledSlider;
class MediaPlayer;
} // namespace greu::ui

namespace greu::tag {
class File;
class Album;
} // namespace greu::tag

namespace greu::grylizo {

class FileEditor;
class FileTable;
class AlbumTable;
class DirectoryTree;

class MainWindow : public ui::MainWindow {
public:
    MainWindow();

    FileEditor *fileEditor() const;
    FileTable *fileTable() const;
    AlbumTable *albumTable() const;
    DirectoryTree *directoryTree() const;
    QTabWidget *treeTabs() const;
    static int const tabAlbums = 0;
    static int const tabFolders = 1;
    ui::MediaPlayer *player() const;

    void setActiveFile(tag::File *);
    void setActiveAlbum(tag::Album *);
    void update();

    virtual void endEditing() override;
    virtual void setStatusMessage(QString const &) override;

    void gotoFile(tag::File *);
    void relativeFile(int step);
    void saveOtherSettings();
    void restoreOtherSettings();

private:
    virtual void setupUiInt() override;
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::grylizo
