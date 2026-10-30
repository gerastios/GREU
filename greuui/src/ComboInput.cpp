#include "ComboInput.h"
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include "ComboBox.h"
#include "Icons.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "utils.h"

using namespace std;
using namespace greu::core;

namespace greu::ui {

struct ComboInput::Detail {
    GuardedPtr<QGridLayout> layout;
    GuardedPtr<QLabel> label;
    GuardedPtr<ComboBox> combo;
    GuardedPtr<QDialogButtonBox> buttons;
    GuardedPtr<QPushButton> ok, cancel;

    void setupUi(ComboInput *that) {
        layout = new QGridLayout;
        addNewWidget(label, layout.get(), 0, 0);
        addNewWidget(combo, layout.get(), 0, 1);
        addNewWidget(buttons, layout.get(), 1, 0, 1, 2);
        ok = new QPushButton(GREU_ICON(ok), "");
        cancel = new QPushButton(GREU_ICON(cancel), "");
        buttons->addButton(ok.get(), QDialogButtonBox::AcceptRole);
        buttons->addButton(cancel.get(), QDialogButtonBox::RejectRole);
        ui::connect(ok.get(), [that] { that->accept(); });
        ui::connect(cancel.get(), [that] { that->reject(); });
    }
};

ComboInput::ComboInput(QString const &labelText, ComboValues const &strings)
    : d(GREU_MKDET()) {
    d->setupUi(this);
    setLayout(d->layout.get());
    for (auto &s : strings)
        d->combo->addItem(s.text, s.data);
    d->label->setText(labelText);
}

QVariant ComboInput::result() const {
    return d->combo->currentData();
}

} // namespace greu::ui
