#include "SettingsDialog.h"
#include <QButtonGroup>
#include <QCheckBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QRadioButton>
#include <QToolBar>
#include <QVBoxLayout>
#include "Settings.h"
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/Action.h"
#include "greuui/include/Icons.h"
#include "greuui/include/LineEdit.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;

struct SettingsDialog::Detail {
    GuardedPtr<QVBoxLayout> mainLayout;
    GuardedPtr<QHBoxLayout> overwriteTabAttLayout;
    GuardedPtr<QGroupBox> dateTimeFormatBox;
    GuardedPtr<QRadioButton> dateFormatADateW, dateFormatEDateW, dateFormatSDateW, dateFormatIso,
        timeFormat24, timeFormatAmPm;
    GuardedPtr<LineEdit> quadLengthEdit;
    QIntValidator quadLengthValidator;
    GuardedPtr<QCheckBox> overwriteTabAtt;
    GuardedPtr<Action> actOk, actCancel;
    GuardedPtr<QGridLayout> dateTimeFormatLayout, quadLengthLayout;
    GuardedPtr<QButtonGroup> dateFormatGroup, timeFormatGroup;
    GuardedPtr<QToolBar> toolBar;
    GuardedPtr<QLabel> quadLengthLabel, overwriteTabAttLabel;

    GuardedPtr<QRadioButton> addRadioButton(
        QString const &text, QButtonGroup *group, QGridLayout *layout, int row, int column) {
        GuardedPtr<QRadioButton> res = new QRadioButton(text);
        group->addButton(res.get());
        layout->addWidget(res.get(), row, column);
        return res;
    }

    void setupDateTimeFormat() {
        dateTimeFormatBox = new QGroupBox(QObject::tr("Date/Time format"));
        dateTimeFormatLayout = new QGridLayout;
        mainLayout->addWidget(dateTimeFormatBox.get());
        dateFormatGroup = new QButtonGroup;
        dateFormatADateW = addRadioButton("mm/dd/yyyy",
                                          dateFormatGroup.get(),
                                          dateTimeFormatLayout.get(),
                                          0,
                                          0);
        dateFormatEDateW = addRadioButton("dd.mm.yyyy",
                                          dateFormatGroup.get(),
                                          dateTimeFormatLayout.get(),
                                          1,
                                          0);
        dateFormatSDateW = addRadioButton("yyyy/mm/dd",
                                          dateFormatGroup.get(),
                                          dateTimeFormatLayout.get(),
                                          2,
                                          0);
        dateFormatIso = addRadioButton("yyyy-mm-dd",
                                       dateFormatGroup.get(),
                                       dateTimeFormatLayout.get(),
                                       3,
                                       0);
        switch (Settings::instance()->dateFormat()) {
        case Settings::DateFormat::ADateW:
            dateFormatADateW->setChecked(true);
            break;
        case Settings::DateFormat::EDateW:
            dateFormatEDateW->setChecked(true);
            break;
        case Settings::DateFormat::SDateW:
            dateFormatSDateW->setChecked(true);
            break;
        case Settings::DateFormat::iso:
            dateFormatIso->setChecked(true);
            break;
        }
        timeFormatGroup = new QButtonGroup;
        timeFormat24 = addRadioButton(QObject::tr("24 hours"),
                                      timeFormatGroup.get(),
                                      dateTimeFormatLayout.get(),
                                      0,
                                      1);
        timeFormatAmPm = addRadioButton(QObject::tr("12 hours (am/pm)"),
                                        timeFormatGroup.get(),
                                        dateTimeFormatLayout.get(),
                                        1,
                                        1);
        switch (Settings::instance()->timeFormat()) {
        case Settings::TimeFormat::_24:
            timeFormat24->setChecked(true);
            break;
        case Settings::TimeFormat::AmPm:
            timeFormatAmPm->setChecked(true);
            break;
        }
    }

    void init(SettingsDialog *that) {
        actOk = new Action(QObject::tr("OK"),
                           QKeySequence(Qt::Key_Enter),
                           QKeySequence(Qt::Key_Return),
                           GREU_ICON(ok));
        actCancel = new Action(QObject::tr("Cancel"),
                               QKeySequence(Qt::Key_Escape),
                               GREU_ICON(cancel));
        mainLayout = new QVBoxLayout;
        that->setLayout(mainLayout.get());
        setupDateTimeFormat();
        toolBar = new QToolBar;
        toolBar->addAction(actOk.get());
        ui::connect(actOk.get(), [this, that] {
            save();
            that->accept();
        });
        ui::connect(actCancel.get(), [that] { that->reject(); });
        toolBar->addAction(actCancel.get());
        setupQuadLength();
        setupOverwriteTabAtt();
        mainLayout->insertStretch(-1);
        mainLayout->addWidget(toolBar.get());
    }

    void save() {
        saveDateFormat();
        saveTimeFormat();
        saveQuadLength();
    }

    void saveDateFormat() {
        if (dateFormatADateW->isChecked())
            Settings::instance()->setDateFormat(Settings::DateFormat::ADateW);
        else if (dateFormatEDateW->isChecked())
            Settings::instance()->setDateFormat(Settings::DateFormat::EDateW);
        else if (dateFormatSDateW->isChecked())
            Settings::instance()->setDateFormat(Settings::DateFormat::SDateW);
        else if (dateFormatIso->isChecked())
            Settings::instance()->setDateFormat(Settings::DateFormat::iso);
    }

    void saveTimeFormat() {
        if (timeFormat24->isChecked())
            Settings::instance()->setTimeFormat(Settings::TimeFormat::_24);
        else if (timeFormatAmPm->isChecked())
            Settings::instance()->setTimeFormat(Settings::TimeFormat::AmPm);
    }

    void saveQuadLength() {
        Settings::instance()->setQuadratLength(quadLengthEdit->text().toInt());
    }

    void saveOverwriteTabAtt() {
        Settings::instance()->setOverwriteTableAttribute(
            overwriteTabAtt->checkState() == Qt::CheckState::PartiallyChecked
                ? nullopt
                : (overwriteTabAtt->checkState() == Qt::CheckState::Checked ? optional(true)
                                                                            : optional(false)));
    }

    void setupQuadLength() {
        quadLengthLayout = new QGridLayout;
        quadLengthLabel = new QLabel("Image quadrat length");
        quadLengthEdit = new LineEdit;
        quadLengthValidator.setRange(0, 2000);
        quadLengthEdit->setValidator(&quadLengthValidator);
        quadLengthEdit->setText(QString::number(Settings::instance()->quadratLength()));
        quadLengthLabel->setBuddy(quadLengthEdit.get());
        quadLengthLayout->addWidget(quadLengthLabel.get(), 0, 0, Qt::AlignLeft);
        quadLengthLayout->addWidget(quadLengthEdit.get(), 0, 1, Qt::AlignRight);
        mainLayout->addLayout(quadLengthLayout.get());
    }

    void setupOverwriteTabAtt() {
        overwriteTabAttLayout = new QHBoxLayout;
        addNewWidget(overwriteTabAttLabel,
                     QObject::tr("Overwrite table attribute on no selection"),
                     overwriteTabAttLayout.get());
        addNewWidget(overwriteTabAtt, overwriteTabAttLayout.get());
        mainLayout->addLayout(overwriteTabAttLayout.get());
        overwriteTabAtt->setTristate(true);
        auto valuec = Settings::instance()->overwriteTableAttribute();
        overwriteTabAtt->setCheckState(
            valuec.has_value()
                ? (valuec.value() ? Qt::CheckState::Checked : Qt::CheckState::Unchecked)
                : Qt::CheckState::PartiallyChecked);
    }
};

SettingsDialog::SettingsDialog()
    : d(GREU_MKDET()) {
    d->init(this);
}

} // namespace greu::grylizo
