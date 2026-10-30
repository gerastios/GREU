QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    include \
    ..

SOURCES += \
    src/Application.cpp \
    src/ChapterEditor.cpp \
    src/ConvertDialog.cpp \
    src/Dispatcher.cpp \
    src/FileListTable.cpp \
    src/MainWindow.cpp \
    src/Media.cpp \
    src/TimeCalculator.cpp \
    src/TrackNumberDialog.cpp \
    src/WorkFileTable.cpp \
    src/dvdInfo.cpp \
    src/main.cpp

HEADERS += \
    include/Application.h \
    include/ChapterEditor.h \
    include/ConvertDialog.h \
    include/Dispatcher.h \
    include/FileListTable.h \
    include/MainWindow.h \
    include/Media.h \
    include/TimeCalculator.h \
    include/TrackNumberDialog.h \
    include/WorkFileTable.h \
    include/dvdInfo.h

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc

DISTFILES += \
    geranos.jpg

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
