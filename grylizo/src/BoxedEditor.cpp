#include "BoxedEditor.h"
#include <QGridLayout>
#include <QGroupBox>
#include "include/GuardedPtr.h"

namespace greu::grylizo {

using namespace core;

struct BoxedEditor::Detail {
    QString const text;
    GuardedPtr<QGroupBox> box;
    GuardedPtr<QVBoxLayout> mainLayout;
    GuardedPtr<QGridLayout> layout;

    Detail(QString const &text)
        : text(text) {}

    QWidget *setupUi(BoxedEditor *that) {
        if (text.length() > 0)
            box = new QGroupBox(text);
        else
            box = new QGroupBox;
        mainLayout = new QVBoxLayout;
        box->setLayout(mainLayout.get());
        layout = new QGridLayout;
        int row = 0;
        for (auto e = that->begin(); e != that->end(); e++) {
            (*e)->setupUi(layout.get(), row++);
        }
        mainLayout->addLayout(layout.get());
        mainLayout->addStretch();
        return box.get();
    }
};

BoxedEditor::BoxedEditor(QString const &text)
    : d(GREU_MKDET(text)) {}

QWidget *BoxedEditor::setupUi() {
    return d->setupUi(this);
}

} // namespace greu::grylizo
