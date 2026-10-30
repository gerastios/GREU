TEMPLATE = app
CONFIG += console c++latest
CONFIG -= app_bundle
QT += core gui widgets multimedia

INCLUDEPATH += \
    include \
    ..

SOURCES += \
    src/Actions.cpp \
    src/AdvancedEditor.cpp \
    src/AlbumTable.cpp \
    src/Application.cpp \
    src/AttributeEditor.cpp \
    src/BoxedEditor.cpp \
    src/CompoundAttributeEditor.cpp \
    src/DateTimeAttributeEditor.cpp \
    src/DirectoryTree.cpp \
    src/Dispatcher.cpp \
    src/DuplicateTool.cpp \
    src/FileEditor.cpp \
    src/FileTable.cpp \
    src/FileTableModel.cpp \
    src/FindTool.cpp \
    src/KeyedListAttributeEditor.cpp \
    src/ListAttributeEditor.cpp \
    src/MainEditor.cpp \
    src/MainWindow.cpp \
    src/NormalizeTool.cpp \
    src/PlainEditor.cpp \
    src/SelectionTextAttributeEditor.cpp \
    src/Settings.cpp \
    src/SettingsDialog.cpp \
    src/TextAttributeEditor.cpp \
    src/ValuedKeydListAttributeEditor.cpp \
    src/main.cpp

unix:!macx: LIBS += -L$$OUT_PWD/../greucore/ -lgreucore

INCLUDEPATH += $$PWD/../greucore
DEPENDPATH += $$PWD/../greucore

unix:!macx: LIBS += -L$$OUT_PWD/../greuid3/ -lgreuid3

INCLUDEPATH += $$PWD/../greuid3
DEPENDPATH += $$PWD/../greuid3

unix:!macx: LIBS += -L$$OUT_PWD/../greutag/ -lgreutag

INCLUDEPATH += $$PWD/../greutag
DEPENDPATH += $$PWD/../greutag

unix:!macx: LIBS += -L$$OUT_PWD/../greuui/ -lgreuui

INCLUDEPATH += $$PWD/../greuui
DEPENDPATH += $$PWD/../greuui

DISTFILES += \
    images/grylizo.png \
    images/splash.odg \
    images/splash.png

RESOURCES += \
    images.qrc

HEADERS += \
    include/AbstractAttributeEditor.h \
    include/Actions.h \
    include/AdvancedEditor.h \
    include/AlbumTable.h \
    include/Application.h \
    include/AttributeEditor.h \
    include/BoxedEditor.h \
    include/CompoundAttributeEditor.h \
    include/DateTimeAttributeEditor.h \
    include/DirectoryTree.h \
    include/Dispatcher.h \
    include/DuplicateTool.h \
    include/FileEditor.h \
    include/FileTable.h \
    include/FileTableModel.h \
    include/FindTool.h \
    include/KeyedListAttributeEditor.h \
    include/ListAttributeEditor.h \
    include/MainEditor.h \
    include/MainWindow.h \
    include/NormalizeTool.h \
    include/PlainEditor.h \
    include/SelectionTextAttributeEditor.h \
    include/Settings.h \
    include/SettingsDialog.h \
    include/TextAttributeEditor.h \
    include/ValuedKeydListAttributeEditor.h
