QT += widgets network sql

TEMPLATE = lib
DEFINES += GREUGAIONAPP_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    include \
    ..

SOURCES += \
    src/Dispatcher.cpp \
    src/MainWindow.cpp \
    src/SearchDialog.cpp

HEADERS += \
    include/Dispatcher.h \
    include/MainWindow.h \
    include/SearchDialog.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    gaionimages.qrc

DISTFILES += \
    images/gaion.jpg \
    images/movies.jpg
