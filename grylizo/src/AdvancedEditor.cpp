#include "AdvancedEditor.h"
#include <QStackedWidget>
#include <QVBoxLayout>
#include "Application.h"
#include "BoxedEditor.h"
#include "DateTimeAttributeEditor.h"
#include "KeyedListAttributeEditor.h"
#include "ListAttributeEditor.h"
#include "SelectionTextAttributeEditor.h"
#include "TextAttributeEditor.h"
#include "ValuedKeydListAttributeEditor.h"
#include "greucore/include/Settings.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/AttributeTypes.h"
#include "greuui/include/ComboBox.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"
#include "include/GuardedPtr.h"
#include "include/connect.h"

using namespace std;

namespace greu::grylizo {

using namespace greu::core;
using namespace greu::ui;
using namespace greu::tag;

struct AdvancedEditor::Detail {
    GuardedPtr<QWidget> widget;
    GuardedPtr<QVBoxLayout> layout;
    GuardedPtr<ComboBox> selection;
    GuardedPtr<QStackedWidget> stack;
    core::Settings settings;

    Detail()
        : settings(Application::instance()->applicationName()) {}

    QWidget *setupUi(AdvancedEditor *that) {
        widget = new QWidget;
        layout = new QVBoxLayout;
        addNewWidget(selection, layout.get());
        addNewWidget(stack, layout.get());
        widget->setLayout(layout.get());

        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::beatsPerMinute()),
                                    TextAttributeEditor::uintAdapter()));
        that->addEditor(makeListEditor(AttributeTypes::type(AttributeTypes::composers()),
                                       ListAttributeEditor::stringAdapter(),
                                       "composers"));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::conductor()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeTimeBox(AttributeTypes::type(AttributeTypes::encodingTime())));
        that->addEditor(makeListEditor(AttributeTypes::type(AttributeTypes::encodedBy()),
                                       ListAttributeEditor::stringAdapter(),
                                       "encodedBy"));
        that->addEditor(makeListEditor(AttributeTypes::type(AttributeTypes::encodingSettings()),
                                       ListAttributeEditor::stringAdapter(),
                                       "encodingSettings"));
        that->addEditor(
            makeValuedKeyedListEditor(AttributeTypes::type(AttributeTypes::eventTimingCodesUnit()),
                                      AttributeTypes::type(AttributeTypes::eventTimingCodesValues()),
                                      ValuedKeydListAttributeEditor::uintUintUintAdapter(),
                                      "eventTimingCodes"));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::fileOwner()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeSelectionTextBox(AttributeTypes::type(AttributeTypes::fileType()),
                                             SelectionTextAttributeEditor::stringAdapter()));
        that->addEditor(makeListEditor(AttributeTypes::type(AttributeTypes::genres()),
                                       ListAttributeEditor::stringAdapter(),
                                       "genres"));
        that->addEditor(makeKeyedListEditor(AttributeTypes::type(AttributeTypes::involvedPeople()),
                                            &KeyedListAttributeEditor::stringStringAdapter,
                                            "involvedPeople"));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::isrc()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::key()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeListEditor(AttributeTypes::type(AttributeTypes::languages()),
                                       ListAttributeEditor::stringAdapter(),
                                       "languages"));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::length()),
                                    TextAttributeEditor::uintAdapter()));
        that->addEditor(makeListEditor(AttributeTypes::type(AttributeTypes::lyricists()),
                                       ListAttributeEditor::stringAdapter(),
                                       "lyricists"));
        that->addEditor(makeSelectionTextBox(AttributeTypes::type(AttributeTypes::mediaType()),
                                             SelectionTextAttributeEditor::stringAdapter()));
        that->addEditor(makeListEditor(AttributeTypes::type(AttributeTypes::moods()),
                                       ListAttributeEditor::stringAdapter(),
                                       "moods"));
        that->addEditor(makeKeyedListEditor(AttributeTypes::type(AttributeTypes::musicians()),
                                            &KeyedListAttributeEditor::stringStringAdapter,
                                            "musicians"));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::originalArtist()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::originalFileName()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeListEditor(AttributeTypes::type(AttributeTypes::originalLyricists()),
                                       ListAttributeEditor::stringAdapter(),
                                       "originalLyricists"));
        that->addEditor(makeTimeBox(AttributeTypes::type(AttributeTypes::originalReleaseTime())));
        that->addEditor(makeKeyedListEditor(AttributeTypes::type(AttributeTypes::otherTexts()),
                                            &KeyedListAttributeEditor::stringStringAdapter,
                                            "otherTexts"));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::playlistDelay()),
                                    TextAttributeEditor::uintAdapter()));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::produced()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::publisher()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::radioStationName()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeTextBox(AttributeTypes::type(AttributeTypes::radioStationOwner()),
                                    TextAttributeEditor::stringAdapter()));
        that->addEditor(makeTimeBox(AttributeTypes::type(AttributeTypes::recordingTime())));
        that->addEditor(makeListEditor(AttributeTypes::type(AttributeTypes::remixedBy()),
                                       ListAttributeEditor::stringAdapter(),
                                       "remixedBy"));
        that->addEditor(makeTimeBox(AttributeTypes::type(AttributeTypes::taggingTime())));
        that->addEditor(
            makeKeyedListEditor(AttributeTypes::type(AttributeTypes::uniqueFileIdentifier()),
                                &KeyedListAttributeEditor::stringBytesAdapter,
                                "uniqueFileIdentifier"));
        that->addEditor(makeKeyedListEditor(AttributeTypes::type(AttributeTypes::urls()),
                                            &KeyedListAttributeEditor::uintStringAdapter,
                                            "urls"));

        connectIndex(selection.get(), [this](auto index) { stack->setCurrentIndex(index); });
        selection->setCurrentIndex(settings.value("AdvancedSelection", 0).toInt());

        return widget.get();
    }

    void addItem(AbstractAttributeEditor *editor, QString const &text) {
        stack->addWidget(editor->setupUi());
        selection->addItem(text);
    }

    unique_ptr<BoxedEditor> makeTextBox(AttributeType *type, TextAttributeEditor::Adapter *adapter) {
        auto res = make_unique<BoxedEditor>();
        auto editor = make_unique<TextAttributeEditor>(type, adapter, true, true);
        res->addEditor(std::move(editor));
        addItem(res.get(), type->longUpper());
        return res;
    }

    unique_ptr<BoxedEditor> makeSelectionTextBox(AttributeType *type,
                                                 SelectionTextAttributeEditor::Adapter *adapter) {
        auto res = make_unique<BoxedEditor>();
        auto editor = make_unique<SelectionTextAttributeEditor>(type, adapter, true, true);
        res->addEditor(std::move(editor));
        addItem(res.get(), type->longUpper());
        return res;
    }

    unique_ptr<KeyedListAttributeEditor> makeKeyedListEditor(
        AttributeType *type,
        KeyedListAttributeEditor::AdapterCreator *adapterCreator,
        QString const &name) {
        auto res = make_unique<KeyedListAttributeEditor>(type, name, adapterCreator);
        addItem(res.get(), type->pluralUpper());
        return res;
    }

    unique_ptr<ValuedKeydListAttributeEditor> makeValuedKeyedListEditor(
        AttributeType *typeUnit,
        AttributeType *typeValues,
        ValuedKeydListAttributeEditor::Adapter *adapter,
        QString const &name) {
        auto res = make_unique<ValuedKeydListAttributeEditor>(typeUnit, typeValues, name, adapter);
        addItem(res.get(), typeValues->pluralUpper());
        return res;
    }

    unique_ptr<ListAttributeEditor> makeListEditor(AttributeType *type,
                                                   ListAttributeEditor::Adapter *adapter,
                                                   QString const &name) {
        auto res = make_unique<ListAttributeEditor>(type, name, adapter);
        addItem(res.get(), type->pluralUpper());
        return res;
    }

    unique_ptr<BoxedEditor> makeTimeBox(AttributeType *type) {
        auto res = make_unique<BoxedEditor>();
        auto editor = make_unique<DateTimeAttributeEditor>(type);
        res->addEditor(std::move(editor));
        addItem(res.get(), type->longUpper());
        return res;
    }

    ~Detail() { settings.setValue("AdvancedSelection", selection->currentIndex()); }
};

AdvancedEditor::AdvancedEditor()
    : d(GREU_MKDET()) {}

QWidget *AdvancedEditor::setupUi() {
    return d->setupUi(this);
}

} // namespace greu::grylizo
