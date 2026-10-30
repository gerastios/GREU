QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    include \
    ..

SOURCES += \
    main.cpp \
    src/Application.cpp \
    src/Dispatcher.cpp \
    src/MainWindow.cpp

HEADERS += \
    include/Application.h \
    include/Dispatcher.h \
    include/MainWindow.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greucore/release/ -lgreucore
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greucore/debug/ -lgreucore
else:unix: LIBS += -L$$OUT_PWD/../greucore/ -lgreucore

INCLUDEPATH += $$PWD/../greucore
DEPENDPATH += $$PWD/../greucore

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../greuui/release/ -lgreuui
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../greuui/debug/ -lgreuui
else:unix: LIBS += -L$$OUT_PWD/../greuui/ -lgreuui

INCLUDEPATH += $$PWD/../greuui
DEPENDPATH += $$PWD/../greuui
