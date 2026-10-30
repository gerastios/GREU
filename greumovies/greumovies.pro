QT += core network sql

TEMPLATE = lib
DEFINES += GREUMOVIES_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    include \
    ..

SOURCES += \
    src/Cleanable.cpp \
    src/Cleanser.cpp \
    src/Collectable.cpp \
    src/Collection.cpp \
    src/CollectionsTable.cpp \
    src/Credit.cpp \
    src/CreditAssignmentsTable.cpp \
    src/CreditsTable.cpp \
    src/EmptyImages.cpp \
    src/Episode.cpp \
    src/EpisodesTable.cpp \
    src/File.cpp \
    src/FilesTable.cpp \
    src/GenresDefinitionTable.cpp \
    src/GenresTable.cpp \
    src/JobsTable.cpp \
    src/Movie.cpp \
    src/MovieCollection.cpp \
    src/MovieCollectionsTable.cpp \
    src/MovieObject.cpp \
    src/MoviesTable.cpp \
    src/Object.cpp \
    src/ObjectState.cpp \
    src/Person.cpp \
    src/PersonsTable.cpp \
    src/Season.cpp \
    src/SeasonsTable.cpp \
    src/Series.cpp \
    src/SeriesTable.cpp \
    src/Settings.cpp \
    src/Strings.cpp \
    src/creditUtils.cpp \
    src/database.cpp \
    src/searchUtils.cpp

HEADERS += \
    include/Cleanable.h \
    include/Cleanser.h \
    include/Collectable.h \
    include/Collection.h \
    include/CollectionsTable.h \
    include/Credit.h \
    include/CreditAssignmentsTable.h \
    include/CreditHolder.h \
    include/CreditsTable.h \
    include/EmptyImages.h \
    include/Episode.h \
    include/EpisodesTable.h \
    include/File.h \
    include/FilesTable.h \
    include/GenresDefinitionTable.h \
    include/GenresTable.h \
    include/JobsTable.h \
    include/Movie.h \
    include/MovieCollection.h \
    include/MovieCollectionsTable.h \
    include/MovieObject.h \
    include/MoviesTable.h \
    include/Object.h \
    include/ObjectState.h \
    include/OwnMovie.h \
    include/Person.h \
    include/PersonsTable.h \
    include/SearchOptions.h \
    include/Searchable.h \
    include/Season.h \
    include/SeasonsTable.h \
    include/Series.h \
    include/SeriesTable.h \
    include/Settings.h \
    include/Strings.h \
    include/Video.h \
    include/creditUtils.h \
    include/creditsDefinitions.h \
    include/database.h \
    include/datatypes.h \
    include/property.h \
    include/searchUtils.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    images/poster.png \
    images/stillimage.png \
    images/unknown.png

RESOURCES += \
    movieimages.qrc
