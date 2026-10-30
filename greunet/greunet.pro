QT += core network

TEMPLATE = lib
DEFINES += GREUNET_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += include \
        ..

SOURCES += \
    src/Exception.cpp \
    src/JsonException.cpp \
    src/JsonRequester.cpp \
    src/Reader.cpp \
    src/Request.cpp

HEADERS += \
    include/Exception.h \
    include/JsonException.h \
    include/JsonRequester.h \
    include/Reader.h \
    include/Request.h \
    include/Requester.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
