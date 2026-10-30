QT += widgets

TEMPLATE = lib
DEFINES += GREUMPV_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += include \
        ..

SOURCES += \
    src/Player.cpp \
    src/PlayerWindow.cpp \
    src/Video.cpp \
    src/utils.cpp

HEADERS += \
    include/Player.h \
    include/PlayerWindow.h \
    include/Video.h \
    include/utils.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
