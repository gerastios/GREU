QT += core sql widgets

TEMPLATE = lib
DEFINES += GREUSQL_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += include \
        ..

SOURCES += \
    src/Connection.cpp \
    src/Exception.cpp \
    src/Query.cpp \
    src/Table.cpp

HEADERS += \
    include/Connection.h \
    include/Exception.h \
    include/Field.h \
    include/Index.h \
    include/Query.h \
    include/Table.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
