QT += gui

TEMPLATE = lib
DEFINES += GREUID3_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += include \
        ..

SOURCES += \
    src/DefaultFrameHandler.cpp \
    src/File.cpp \
    src/FileCreator.cpp \
    src/FileTypeSelector.cpp \
    src/Frame.cpp \
    src/FrameHandlerRegistry.cpp \
    src/ImageTypeSelector.cpp \
    src/MediaTypeSelector.cpp \
    src/Reader.cpp \
    src/Tag.cpp \
    src/UrlTypeSelector.cpp \
    src/Writer.cpp

HEADERS += \
    greuid3_global.h \
    include/DefaultFrameHandler.h \
    include/File.h \
    include/FileCreator.h \
    include/FileTypeSelector.h \
    include/Frame.h \
    include/FrameHandler.h \
    include/FrameHandlerRegistry.h \
    include/ImageTypeSelector.h \
    include/MediaTypeSelector.h \
    include/Reader.h \
    include/Tag.h \
    include/UrlTypeSelector.h \
    include/Writer.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
