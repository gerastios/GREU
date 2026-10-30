QT += core network

TEMPLATE = lib
DEFINES += GREUTMDB_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    include \
    ..

SOURCES += \
    src/CreditRequester.cpp \
    src/EpisodeRequester.cpp \
    src/ImageReader.cpp \
    src/MovieCollectionRequester.cpp \
    src/MovieRequester.cpp \
    src/PersonRequester.cpp \
    src/Reader.cpp \
    src/Request.cpp \
    src/SeasonRequester.cpp \
    src/SeriesRequester.cpp \
    src/TheMovieDatabase.cpp \
    src/TmdbRequester.cpp

HEADERS += \
    include/Credit.h \
    include/CreditRequester.h \
    include/EpisodeRequester.h \
    include/ImageReader.h \
    include/JsonRequester.h \
    include/MovieCollectionRequester.h \
    include/MovieRequester.h \
    include/PersonRequester.h \
    include/Reader.h \
    include/Request.h \
    include/Requester.h \
    include/SeasonRequester.h \
    include/SeriesRequester.h \
    include/TheMovieDatabase.h \
    include/TmdbRequester.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
