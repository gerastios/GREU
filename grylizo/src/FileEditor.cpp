#include "FileEditor.h"
#include <QTabWidget>
#include <QVBoxLayout>
#include "AdvancedEditor.h"
#include "Dispatcher.h"
#include "KeyedListAttributeEditor.h"
#include "MainEditor.h"
#include "MainWindow.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/vectorUtils.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/AttributeTypes.h"
#include "greutag/include/File.h"

using namespace std;
;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct FileEditor::Detail {
    File *file = nullptr;
    FileSig fileSet;
    GuardedPtr<QWidget> widget;
    GuardedPtr<QTabWidget> tab;
    GuardedPtr<QVBoxLayout> layout;
    unique_ptr<MainEditor> mainEditor;
    unique_ptr<KeyedListAttributeEditor> imagesEditor;
    unique_ptr<AdvancedEditor> advancedEditor;
    vector<QChar> mnemonics;
    ScopedConnection fileConnection;

    QWidget *setupUi() {
        widget = new QWidget;
        layout = new QVBoxLayout;
        widget->setLayout(layout.get());
        tab = new QTabWidget;
        tab->setObjectName("FileTabWidget");
        mainEditor = std::make_unique<MainEditor>();
        tab->addTab(mainEditor->setupUi(), QObject::tr("Main attributes"));
        imagesEditor
            = std::make_unique<KeyedListAttributeEditor>(tag::AttributeTypes::type(
                                                             tag::AttributeTypes::images()),
                                                         "Images",
                                                         &KeyedListAttributeEditor::imagesAdapter);
        tab->addTab(imagesEditor->setupUi(),
                    tag::AttributeTypes::type(tag::AttributeTypes::images())->pluralUpper());
        advancedEditor = std::make_unique<AdvancedEditor>();
        tab->addTab(advancedEditor->setupUi(), QObject::tr("Advanced attributes"));
        layout->addWidget(tab.get());
        return widget.get();
    }

    void commit() {
        mainEditor->commit();
        imagesEditor->commit();
        advancedEditor->commit();
    }

    void update() {
        if (fileConnection.connected())
            fileConnection.disconnect();
        if (file) {
            fileConnection = file->modifiedChanged().connect([this](auto) { update(); });
        }
        mainEditor->update();
        imagesEditor->update();
        advancedEditor->update();
    }

    void setFile(File *value) {
        if (file != value) {
            if (file)
                commit();
            file = value;
            fileSet(value);
        }
        update();
    }

    bool addMnemonic(QChar const &mnemonic) {
        if (core::contains(mnemonics, mnemonic))
            return false;
        mnemonics.push_back(mnemonic);
        return true;
    }
};

FileEditor::FileEditor()
    : d(GREU_MKDET()) {}

QWidget *FileEditor::setupUi() {
    return d->setupUi();
}

void FileEditor::commit() {
    d->commit();
}

void FileEditor::update() {
    d->update();
}

void FileEditor::setFile(File *v) {
    d->setFile(v);
}

File *FileEditor::file() const {
    return d->file;
}

MainEditor *FileEditor::mainEditor() const {
    return d->mainEditor.get();
}

KeyedListAttributeEditor *FileEditor::imagesEditor() const {
    return d->imagesEditor.get();
}

AdvancedEditor *FileEditor::advancedEditor() const {
    return d->advancedEditor.get();
}

bool FileEditor::addMnemonic(QChar const &mnemonic) {
    return d->addMnemonic(mnemonic);
}

FileEditor::FileSig &FileEditor::fileSet() const {
    return d->fileSet;
}

} // namespace greu::grylizo
