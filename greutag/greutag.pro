QT += gui
QT += core 

TEMPLATE = lib
DEFINES += GREUTAG_LIBRARY

CONFIG += c++latest

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += include \
	..

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target

HEADERS += \
    include/Album.h \
    include/AlbumChanger.h \
    include/Attribute.h \
    include/AttributeType.h \
    include/AttributeTypes.h \
    include/AudioCollection.h \
    include/CommonSelectors.h \
    include/DateTimeAttribute.h \
    include/Directory.h \
    include/File.h \
    include/FileContainer.h \
    include/FileCreator.h \
    include/FileCreatorRegistry.h \
    include/FileHolder.h \
    include/FileNameValidator.h \
    include/ImagesAttribute.h \
    include/KeyedListAttribute.h \
    include/ListAttribute.h \
    include/PlainAttribute.h \
    include/Tag.h \
    include/ValuedKeyedListAttribute.h \
    include/attributeTypeMeta.h

SOURCES += \
    src/Album.cpp \
    src/AlbumChanger.cpp \
    src/Attribute.cpp \
    src/AttributeType.cpp \
    src/AttributeTypes.cpp \
    src/AudioCollection.cpp \
    src/CommonSelectors.cpp \
    src/DateTimeAttribute.cpp \
    src/Directory.cpp \
    src/File.cpp \
    src/FileCreatorRegistry.cpp \
    src/FileNameValidator.cpp \
    src/Tag.cpp
