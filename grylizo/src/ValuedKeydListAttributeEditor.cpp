#include "ValuedKeydListAttributeEditor.h"
#include <QGridLayout>
#include <QWidget>
#include "KeyedListAttributeEditor.h"
#include "SelectionTextAttributeEditor.h"
#include "include/GuardedPtr.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct ValuedKeydListAttributeEditor::Adapter {
    virtual KeyedListAttributeEditor::AdapterCreator *valuesAdapterCreator() const = 0;
    virtual SelectionTextAttributeEditor::Adapter *unitAdapter() const = 0;
};

namespace {

struct UintUintUintAdapter : public virtual ValuedKeydListAttributeEditor::Adapter {
    static UintUintUintAdapter *instance() {
        static UintUintUintAdapter i;
        return &i;
    }

    virtual KeyedListAttributeEditor::AdapterCreator *valuesAdapterCreator() const override {
        return &KeyedListAttributeEditor::uintUintAdapter;
    }

    virtual SelectionTextAttributeEditor::Adapter *unitAdapter() const override {
        return SelectionTextAttributeEditor::uintAdapter();
    }
};

} // namespace

struct ValuedKeydListAttributeEditor::Detail {
    AttributeType *typeUnit, *typeValues;
    QString const name;
    Adapter *adapter;
    GuardedPtr<QWidget> widget;
    GuardedPtr<QGridLayout> layout;

    Detail(AttributeType *typeUnit, AttributeType *typeValues, QString const &name, Adapter *adapter)
        : typeUnit(typeUnit)
        , typeValues(typeValues)
        , name(name)
        , adapter(adapter) {}

    QWidget *setupUi(ValuedKeydListAttributeEditor *that) {
        widget = new QWidget;
        layout = new QGridLayout;
        widget->setLayout(layout.get());
        auto ue = make_unique<SelectionTextAttributeEditor>(typeUnit, adapter->unitAdapter());
        ue->setupUi(layout.get(), 0);
        auto ve = make_unique<KeyedListAttributeEditor>(typeValues,
                                                        name,
                                                        adapter->valuesAdapterCreator());
        layout->addWidget(ve->setupUi(), 1, 0, 1, 5);
        that->addEditor(std::move(ue));
        that->addEditor(std::move(ve));
        that->update();
        return widget.get();
    }
};

ValuedKeydListAttributeEditor::ValuedKeydListAttributeEditor(AttributeType *typeUnit,
                                                             AttributeType *typeValues,
                                                             QString const &name,
                                                             Adapter *adapter)
    : d(GREU_MKDET(typeUnit, typeValues, name, adapter)) {}

QWidget *ValuedKeydListAttributeEditor::setupUi() {
    return d->setupUi(this);
}

ValuedKeydListAttributeEditor::Adapter *ValuedKeydListAttributeEditor::uintUintUintAdapter() {
    return UintUintUintAdapter::instance();
}

} // namespace greu::grylizo
