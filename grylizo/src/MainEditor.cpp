#include "MainEditor.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include "Application.h"
#include "BoxedEditor.h"
#include "DateTimeAttributeEditor.h"
#include "Dispatcher.h"
#include "FileEditor.h"
#include "FileTable.h"
#include "MainWindow.h"
#include "TextAttributeEditor.h"
#include "greutag/include/Album.h"
#include "greutag/include/AlbumChanger.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/AttributeTypes.h"
#include "greutag/include/File.h"
#include "greutag/include/FileNameValidator.h"
#include "greutag/include/Tag.h"
#include "include/GuardedPtr.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct MainEditor::Detail {
    GuardedPtr<QWidget> widget;
    GuardedPtr<QVBoxLayout> layout;
    AlbumChanger *albumChanger = nullptr;
    ScopedConnection connAlbumTitle, connAlbumArtist;

    QWidget *setupUi(MainEditor *that) {
        widget = new QWidget;
        layout = new QVBoxLayout;
        layout->insertStretch(-1);
        widget->setLayout(layout.get());
        addFileAttributes(that);
        layout->insertStretch(-1);
        addAlbumAttributes(that);
        layout->insertStretch(-1);
        addSongAttributes(that);
        layout->insertStretch(-1);
        return widget.get();
    }

    void update() {
        auto file = GREU_M->fileEditor()->file();
        if (file) {
            auto fileNameValidator = dynamic_cast<FileNameValidator *>(
                file->tag()->attributeFileName()->validator());
            if (fileNameValidator)
                fileNameValidator->setFile(file);
            auto tag = file->tag();
            if (tag) {
                connAlbumArtist.disconnect();
                connAlbumTitle.disconnect();
                connAlbumArtist = tag->attributeAlbumArtist()->modifiedChanged().connect(
                    [this, tag](auto) { albumKeyChanged(tag->attributeAlbumArtist()->type()); },
                    boost::signals2::at_front);
                connAlbumTitle = tag->attributeAlbumTitle()->modifiedChanged().connect(
                    [this, tag](auto) { albumKeyChanged(tag->attributeAlbumTitle()->type()); },
                    boost::signals2::at_front);
            }
        }
    }

    void albumChange(AlbumChanger *changer) {
        albumChanger = changer;
        changer->execute();
        albumChanger = nullptr;
        if (ranges::find_if(changer->files(),
                            [](File *f) { return f == GREU_M->fileEditor()->file(); })
            != changer->files().end())
            GREU_M->update();
        GREU_M->setActiveFile(changer->mainFile());
    }

    void addFileAttributes(MainEditor *that) {
        auto box = make_unique<BoxedEditor>(QObject::tr("File attributes"));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::fileName()),
                                             TextAttributeEditor::stringAdapter(),
                                             false,
                                             false));

        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::fileExtension()),
                                             TextAttributeEditor::stringAdapter(),
                                             false,
                                             false));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::filePath()),
                                             TextAttributeEditor::stringAdapter(),
                                             false,
                                             false));
        layout->addWidget(box->setupUi());
        that->addEditor(std::move(box));
    }

    void addAlbumAttributes(MainEditor *that) {
        auto box = make_unique<BoxedEditor>(QObject::tr("Album attributes"));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::albumTitle()),
                                             TextAttributeEditor::stringAdapter(),
                                             false));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::albumArtist()),
                                             TextAttributeEditor::stringAdapter(),
                                             false));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::albumTitleSort()),
                                             TextAttributeEditor::stringAdapter(),
                                             false));
        box->addEditor(make_unique<TextAttributeEditor>(AttributeTypes::type(
                                                            AttributeTypes::albumArtistSort()),
                                                        TextAttributeEditor::stringAdapter(),
                                                        false));
        layout->addWidget(box->setupUi());
        that->addEditor(std::move(box));
    }

    void addSongAttributes(MainEditor *that) {
        auto box = make_unique<BoxedEditor>(QObject::tr("Song attributes"));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::songTitle()),
                                             TextAttributeEditor::stringAdapter()));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::songArtist()),
                                             TextAttributeEditor::stringAdapter()));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::trackNumber()),
                                             TextAttributeEditor::uintAdapter(),
                                             false,
                                             false));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::discNumber()),
                                             TextAttributeEditor::uintAdapter(),
                                             true,
                                             true));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::songTitleSort()),
                                             TextAttributeEditor::stringAdapter()));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::setSubtitle()),
                                             TextAttributeEditor::stringAdapter()));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::songCategory()),
                                             TextAttributeEditor::stringAdapter()));
        box->addEditor(
            make_unique<TextAttributeEditor>(AttributeTypes::type(AttributeTypes::songSubtitle()),
                                             TextAttributeEditor::stringAdapter()));
        box->addEditor(make_unique<TextAttributeEditor>(AttributeTypes::type(
                                                            AttributeTypes::originalAlbumTitle()),
                                                        TextAttributeEditor::stringAdapter()));
        box->addEditor(make_unique<DateTimeAttributeEditor>(
            AttributeTypes::type(AttributeTypes::releaseTime())));
        layout->addWidget(box->setupUi());
        that->addEditor(std::move(box));
    }

    void albumKeyChanged(AttributeType const *const type) {
        auto file = GREU_M->fileEditor()->file();
        if (!file)
            return;
        auto container = GREU_M->fileTable()->fileContainer();
        if (!container)
            return;
        QString containerText;
        auto a = dynamic_cast<Album *>(container);
        if (a)
            containerText = QObject::tr("album");
        else
            containerText = QObject::tr("folder");
        auto att = dynamic_cast<PlainAttribute<QString> *>(file->tag()->attribute(type->id()));
        if (!att)
            return;
        if (!att->isModified())
            return;
        if (castAlbumKeyChange(type->longLower(), att->value(), containerText, *container)) {
            copyAlbumKey(file, container);
        } else {
            AlbumChanger changer{Album::buildKey(file->tag()),
                                 vector<File *>{file},
                                 GREU_D->audioCollection()};
            changer.setMainFile(file);
            albumChange(&changer);
            GREU_M->setActiveAlbum(file->album());
        }
    }

    void albumAttributeChanged(AttributeType *type) {
        auto file = GREU_M->fileEditor()->file();
        if (!file)
            return;
        auto container = GREU_M->fileTable()->fileContainer();
        if (!container)
            return;
        auto att = file->tag()->attribute(type->id());
        if (!att)
            return;
        if (!att->isModified())
            return;
        for (auto f = container->begin(); f != container->end(); ++f) {
            if (*f != file) {
                auto to = (*f)->tag()->attribute(type->id());
                if (to) {
                    to->copyValue(*att);
                }
            }
        }
    }

    void copyAlbumKey(File *from, FileContainer *container) {
        vector<File *> files;
        for (auto f = container->begin(); f != container->end(); f++) {
            files.push_back(*f);
        }
        AlbumChanger changer{Album::buildKey(from->tag()), files, GREU_D->audioCollection()};
        changer.setMainFile(from);
        albumChange(&changer);
        GREU_M->setActiveAlbum(from->album());
    }

    bool castAlbumKeyChange(QString const &att,
                            QString const &value,
                            QString const &container,
                            FileContainer const &oldFileContainer) {
        if (oldFileContainer.size() < 2)
            return false;
        QMessageBox box;
        box.setText(QObject::tr("Should the %1 \"%2\" be casted to all files of the %3?")
                        .arg(att, value, container));
        box.setStandardButtons(QMessageBox::Yes | QMessageBox::No);
        box.setDefaultButton(QMessageBox::Yes);
        auto a = box.exec();
        switch (a) {
        case QMessageBox::Yes:
            return true;
        case QMessageBox::No:
            return false;
        }
        return false;
    }
};

MainEditor::MainEditor()
    : CompoundAttributeEditor()
    , d(GREU_MKDET()) {}

QWidget *MainEditor::setupUi() {
    return d->setupUi(this);
}

void MainEditor::update() {
    d->update();
    CompoundAttributeEditor::update();
}

void MainEditor::albumChange(AlbumChanger *changer) {
    d->albumChange(changer);
}

} // namespace greu::grylizo
