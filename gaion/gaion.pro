QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

QT_CONFIG -= no-pkg-config
CONFIG += link_pkgconfig
PKGCONFIG += mpv

INCLUDEPATH += \
    include \
    ..

SOURCES += \
    src/Application.cpp \
    src/main.cpp

HEADERS += \
    include/Application.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greucore/release/ -lgreucore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greucore/debug/ -lgreucore
else:unix: LIBS += -L$$OUT_PWD/../greucore/ -lgreucore

INCLUDEPATH += $$PWD/../greucore
DEPENDPATH += $$PWD/../greucore

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greugaion/release/ -lgreugaion
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greugaion/debug/ -lgreugaion
else:unix: LIBS += -L$$OUT_PWD/../greugaion/ -lgreugaion

INCLUDEPATH += $$PWD/../greugaion
DEPENDPATH += $$PWD/../greugaion

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greugaionapp/release/ -lgreugaionapp
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greugaionapp/debug/ -lgreugaionapp
else:unix: LIBS += -L$$OUT_PWD/../greugaionapp/ -lgreugaionapp

INCLUDEPATH += $$PWD/../greugaionapp
DEPENDPATH += $$PWD/../greugaionapp

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greugaionsetup/release/ -lgreugaionsetup
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greugaionsetup/debug/ -lgreugaionsetup
else:unix: LIBS += -L$$OUT_PWD/../greugaionsetup/ -lgreugaionsetup

INCLUDEPATH += $$PWD/../greugaionsetup
DEPENDPATH += $$PWD/../greugaionsetup

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greumovies/release/ -lgreumovies
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greumovies/debug/ -lgreumovies
else:unix: LIBS += -L$$OUT_PWD/../greumovies/ -lgreumovies

INCLUDEPATH += $$PWD/../greumovies
DEPENDPATH += $$PWD/../greumovies

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greumpv/release/ -lgreumpv
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greumpv/debug/ -lgreumpv
else:unix: LIBS += -L$$OUT_PWD/../greumpv/ -lgreumpv

INCLUDEPATH += $$PWD/../greumpv
DEPENDPATH += $$PWD/../greumpv

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greunet/release/ -lgreunet
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greunet/debug/ -lgreunet
else:unix: LIBS += -L$$OUT_PWD/../greunet/ -lgreunet

INCLUDEPATH += $$PWD/../greunet
DEPENDPATH += $$PWD/../greunet

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greusql/release/ -lgreusql
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greusql/debug/ -lgreusql
else:unix: LIBS += -L$$OUT_PWD/../greusql/ -lgreusql

INCLUDEPATH += $$PWD/../greusql
DEPENDPATH += $$PWD/../greusql

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greutmdb/release/ -lgreutmdb
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greutmdb/debug/ -lgreutmdb
else:unix: LIBS += -L$$OUT_PWD/../greutmdb/ -lgreutmdb

INCLUDEPATH += $$PWD/../greutmdb
DEPENDPATH += $$PWD/../greutmdb

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greuui/release/ -lgreuui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greuui/debug/ -lgreuui
else:unix: LIBS += -L$$OUT_PWD/../greuui/ -lgreuui

INCLUDEPATH += $$PWD/../greuui
DEPENDPATH += $$PWD/../greuui
