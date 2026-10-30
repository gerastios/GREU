QT += widgets network sql

TEMPLATE = lib
DEFINES += GREUGAION_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += \
    include \
    ..

SOURCES += \
    src/CollectionEntryWidget.cpp \
    src/CollectionItem.cpp \
    src/CollectionWidget.cpp \
    src/CreditWidget.cpp \
    src/Dispatcher.cpp \
    src/EpisodeItem.cpp \
    src/EpisodeWidget.cpp \
    src/FeatureWidget.cpp \
    src/HeadInfoWidget.cpp \
    src/HeadWidget.cpp \
    src/LinkController.cpp \
    src/ListItem.cpp \
    src/ListWidget.cpp \
    src/MovieCollectionItem.cpp \
    src/MovieCollectionWidget.cpp \
    src/MovieItem.cpp \
    src/MovieObjectItem.cpp \
    src/MovieObjectWidget.cpp \
    src/MovieWidget.cpp \
    src/NavigationController.cpp \
    src/PersonItem.cpp \
    src/PersonWidget.cpp \
    src/PlayableFileWidgets.cpp \
    src/Player.cpp \
    src/Presentation.cpp \
    src/PresentationItem.cpp \
    src/PresentationWidget.cpp \
    src/SeasonItem.cpp \
    src/SeasonWidget.cpp \
    src/SeriesItem.cpp \
    src/SeriesWidget.cpp \
    src/Styles.cpp

HEADERS += \
    include/CollectionEntryWidget.h \
    include/CollectionItem.h \
    include/CollectionWidget.h \
    include/CreditWidget.h \
    include/Dispatcher.h \
    include/EpisodeItem.h \
    include/EpisodeWidget.h \
    include/FeatureModel.h \
    include/FeatureWidget.h \
    include/HeadInfoWidget.h \
    include/HeadWidget.h \
    include/LinkController.h \
    include/ListItem.h \
    include/ListWidget.h \
    include/MainWindow.h \
    include/MovieCollectionItem.h \
    include/MovieCollectionWidget.h \
    include/MovieItem.h \
    include/MovieObjectItem.h \
    include/MovieObjectWidget.h \
    include/MovieWidget.h \
    include/NavigationController.h \
    include/PersonItem.h \
    include/PersonWidget.h \
    include/PlayableFileWidgets.h \
    include/Player.h \
    include/Presentation.h \
    include/PresentationItem.h \
    include/PresentationWidget.h \
    include/SeasonItem.h \
    include/SeasonWidget.h \
    include/SeriesItem.h \
    include/SeriesWidget.h \
    include/Styles.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    gaionimg.qrc

DISTFILES += \
    images/movies.jpg
