QT += widgets sql network

TEMPLATE = lib
DEFINES += GREUGAIONSETUP_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    include \
    ..

SOURCES += \
    src/Actions.cpp \
    src/BasePathDialog.cpp \
    src/CollectionDialog.cpp \
    src/CollectionEditor.cpp \
    src/Dispatcher.cpp \
    src/IdPopup.cpp \
    src/ImageImporter.cpp \
    src/MainWindow.cpp \
    src/OwnCreditsEditor.cpp \
    src/OwnGenresEditor.cpp \
    src/OwnMovieEditor.cpp \
    src/SettingsPopup.cpp \
    src/initilializeDb.cpp

HEADERS += \
    include/Actions.h \
    include/BasePathDialog.h \
    include/CollectionDialog.h \
    include/CollectionEditor.h \
    include/Dispatcher.h \
    include/IdPopup.h \
    include/ImageImporter.h \
    include/MainWindow.h \
    include/OwnCreditsEditor.h \
    include/OwnGenresEditor.h \
    include/OwnMovieEditor.h \
    include/SettingsPopup.h \
    include/initilializeDb.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
