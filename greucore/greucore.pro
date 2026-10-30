QT -= gui
QT += core core5compat

TEMPLATE = lib
DEFINES += GREUCORE_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += include

SOURCES += \
    src/Exception.cpp \
    src/FileException.cpp \
    src/Language.cpp \
    src/Log.cpp \
    src/LongRunnable.cpp \
    src/Modifiable.cpp \
    src/ModifiableCollection.cpp \
    src/PlainSelector.cpp \
    src/Restorable.cpp \
    src/Runnable.cpp \
    src/Saver.cpp \
    src/Selector.cpp \
    src/StandardPaths.cpp \
    src/StaticSelector.cpp \
    src/Time.cpp \
    src/Timer.cpp \
    src/Uuid.cpp \
    src/hexUtils.cpp \
    src/intUtils.cpp \
    src/jsonUtils.cpp \
    src/signalUtils.cpp \
    src/stringUtils.cpp

HEADERS += \
    include/AddonRegistry.h \
    include/DetailPtr.h \
    include/Exception.h \
    include/FileException.h \
    include/GuardedPtr.h \
    include/Language.h \
    include/Log.h \
    include/LongRunnable.h \
    include/MimeDataVector.h \
    include/Modifiable.h \
    include/ModifiableCollection.h \
    include/PlainSelector.h \
    include/Restorable.h \
    include/Runnable.h \
    include/Saver.h \
    include/Selector.h \
    include/Settings.h \
    include/StandardPaths.h \
    include/StaticSelector.h \
    include/Time.h \
    include/Timer.h \
    include/Uuid.h \
    include/greuGlobal.h \
    include/hashFunctions.h \
    include/hexUtils.h \
    include/intUtils.h \
    include/jsonUtils.h \
    include/signalUtils.h \
    include/stringUtils.h \
    include/utils.h \
    include/vectorUtils.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
