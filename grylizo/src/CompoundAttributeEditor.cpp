#include "CompoundAttributeEditor.h"

using namespace std;

namespace greu::grylizo {

using namespace core;

struct CompoundAttributeEditor::Detail {
    vector<unique_ptr<AbstractAttributeEditor>> editors;

    AbstractAttributeEditor *addEditor(unique_ptr<AbstractAttributeEditor> &&editor) {
        assert(editor && "No editor");
        auto res = editor.get();
        editors.push_back(std::move(editor));
        return res;
    }

    void update() {
        for (auto &e : editors) {
            e->update();
        }
    }

    void commit() {
        for (auto &e : editors) {
            e->commit();
        }
    }
};

CompoundAttributeEditor::CompoundAttributeEditor()
    : d(GREU_MKDET()) {}

AbstractAttributeEditor *CompoundAttributeEditor::addEditor(
    unique_ptr<AbstractAttributeEditor> &&editor) {
    return d->addEditor(std::move(editor));
}

void CompoundAttributeEditor::update() {
    d->update();
}

void CompoundAttributeEditor::commit() {
    d->commit();
}

vector<unique_ptr<AbstractAttributeEditor>>::iterator CompoundAttributeEditor::begin() {
    return d->editors.begin();
}

vector<unique_ptr<AbstractAttributeEditor>>::iterator CompoundAttributeEditor::end() {
    return d->editors.end();
}

} // namespace greu::grylizo
