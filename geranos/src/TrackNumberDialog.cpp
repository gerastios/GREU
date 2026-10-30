#include "TrackNumberDialog.h"
#include <QGridLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QPushButton>
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/Icons.h"
#include "greuui/include/LineEdit.h"
#include "greuui/include/Settings.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

namespace greu::geranos {

using namespace core;
using namespace ui;

struct TrackNumberDialog::Detail {
    GuardedPtr<QLabel> trackLabel, deviceLabel;
    GuardedPtr<LineEdit> trackEdit, deviceEdit;
    GuardedPtr<QGridLayout> layout;
    GuardedPtr<QHBoxLayout> buttonLayout;
    GuardedPtr<QPushButton> ok, cancel;
    GuardedPtr<QIntValidator> validator;
    int type;

    void setupUi(TrackNumberDialog *that) {
        layout = new QGridLayout;

        buttonLayout = new QHBoxLayout;
        addNewWidget(ok, GREU_ICON(ok), "", buttonLayout.get());
        ui::connect(ok.get(), [that] { that->accept(); });
        ok->setDefault(true);
        addNewWidget(cancel, GREU_ICON(cancel), "", buttonLayout.get());
        ui::connect(cancel.get(), [that] { that->reject(); });

        addNewWidget(trackLabel, QObject::tr("Track number"), layout.get(), 0, 0);
        addNewWidget(trackEdit, layout.get(), 0, 1);
        trackLabel->setBuddy(trackEdit.get());
        validator = new QIntValidator;
        validator->setBottom(1);
        trackEdit->setValidator(validator.get());

        addNewWidget(deviceLabel, QObject::tr("optical device"), layout.get(), 1, 0);
        addNewWidget(deviceEdit, layout.get(), 1, 1);
        deviceEdit->setText(ui::Settings::instance().value("opticalDevice", "/dev/sr0").toString());
        deviceLabel->setBuddy(deviceEdit.get());
        if (!(type & TrackNumber)) {
            trackLabel->setVisible(false);
            trackEdit->setVisible(false);
        }
        if (!(type & Device)) {
            deviceLabel->setVisible(false);
            deviceEdit->setVisible(false);
        }
        layout->addLayout(buttonLayout.get(), 2, 0, 1, 2);
    }
};

TrackNumberDialog::TrackNumberDialog(int t)
    : d(GREU_MKDET()) {
    d->type = t;
}

void TrackNumberDialog::setupUi() {
    d->setupUi(this);
    setLayout(d->layout.get());
}

int TrackNumberDialog::trackNumber() const {
    return d->trackEdit->text().toInt();
}

QString TrackNumberDialog::device() const {
    ui::Settings::instance().setValue("opticalDevice", d->deviceEdit->text());
    return d->deviceEdit->text();
}

} // namespace greu::geranos
