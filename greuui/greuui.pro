QT += gui
QT += widgets
QT += multimedia
QT += core
QT += network

TEMPLATE = lib
DEFINES += GREUUI_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += include \
	..

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images_greuui.qrc

DISTFILES += \
    images/about.png \
    images/add.png \
    images/add_tag.png \
    images/assign.png \
    images/back.png \
    images/cancel.png \
    images/close.png \
    images/convert.png \
    images/copy.png \
    images/crop.png \
    images/double_down.png \
    images/double_up.png \
    images/down.png \
    images/erase.png \
    images/error.png \
    images/find.png \
    images/folder.png \
    images/forward.png \
    images/hide.png \
    images/image.png \
    images/import.png \
    images/inspect.png \
    images/movie.png \
    images/normalize.png \
    images/ok.png \
    images/open.png \
    images/opened_folder.png \
    images/paste.png \
    images/pause.png \
    images/play.png \
    images/quadrat.png \
    images/remove_gaps.png \
    images/remove_tag.png \
    images/resize.png \
    images/save.png \
    images/save_all.png \
    images/save_as.png \
    images/search.png \
    images/select_all.png \
    images/select_none.png \
    images/settings.png \
    images/show.png \
    images/shuffle.png \
    images/stop.png \
    images/subtract.png \
    images/tag_album.png \
    images/tag_container.png \
    images/tag_folder.png \
    images/tag_selected.png \
    images/tags.png \
    images/tv.png \
    images/twins.png \
    images/undo.png \
    images/up.png \
    images/update.png \
    images/volume_high.png \
    images/volume_low.png

HEADERS += \
    include/Accordion.h \
    include/Action.h \
    include/Application.h \
    include/BusyCursor.h \
    include/ComboBox.h \
    include/ComboInput.h \
    include/DateEdit.h \
    include/DateRangeEdit.h \
    include/DateTimeEdit.h \
    include/Dialog.h \
    include/Dispatcher.h \
    include/EditAlerter.h \
    include/EditableTable.h \
    include/EditableTableModel.h \
    include/ExpandingComboBox.h \
    include/ExpandingLineEdit.h \
    include/FileSystemModel.h \
    include/HexEdit.h \
    include/Icons.h \
    include/ImageButton.h \
    include/ImageContainer.h \
    include/ImageLabel.h \
    include/ImageTool.h \
    include/ItemComboBoxDelegate.h \
    include/ItemDelegate.h \
    include/ItemLineEditDelegate.h \
    include/LabelledSlider.h \
    include/LengthValidator.h \
    include/LineEdit.h \
    include/LogViewer.h \
    include/LongRunnableDialog.h \
    include/MainWindow.h \
    include/MediaPlayer.h \
    include/Notepad.h \
    include/ProcessViewer.h \
    include/SavableDispatcher.h \
    include/Separator.h \
    include/Settings.h \
    include/Slider.h \
    include/SplashScreen.h \
    include/Splitter.h \
    include/TableView.h \
    include/TextBrowser.h \
    include/VanishingCursor.h \
    include/connect.h \
    include/iconsMeta.h \
    include/imageTransformation.h \
    include/utils.h

SOURCES += \
    src/Accordion.cpp \
    src/Action.cpp \
    src/Application.cpp \
    src/BusyCursor.cpp \
    src/ComboBox.cpp \
    src/ComboInput.cpp \
    src/DateEdit.cpp \
    src/DateRangeEdit.cpp \
    src/DateTimeEdit.cpp \
    src/Dialog.cpp \
    src/Dispatcher.cpp \
    src/EditableTable.cpp \
    src/ExpandingComboBox.cpp \
    src/ExpandingLineEdit.cpp \
    src/FileSystemModel.cpp \
    src/HexEdit.cpp \
    src/Icons.cpp \
    src/ImageButton.cpp \
    src/ImageContainer.cpp \
    src/ImageLabel.cpp \
    src/ImageTool.cpp \
    src/ItemComboBoxDelegate.cpp \
    src/ItemDelegate.cpp \
    src/ItemLineEditDelegate.cpp \
    src/LabelledSlider.cpp \
    src/LengthValidator.cpp \
    src/LineEdit.cpp \
    src/LogViewer.cpp \
    src/LongRunnableDialog.cpp \
    src/MainWindow.cpp \
    src/MediaPlayer.cpp \
    src/Notepad.cpp \
    src/ProcessViewer.cpp \
    src/SavableDispatcher.cpp \
    src/Separator.cpp \
    src/Settings.cpp \
    src/Slider.cpp \
    src/SplashScreen.cpp \
    src/Splitter.cpp \
    src/TableView.cpp \
    src/TextBrowser.cpp \
    src/VanishingCursor.cpp \
    src/connect.cpp \
    src/imageTransformation.cpp
