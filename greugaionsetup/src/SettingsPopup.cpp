#include "SettingsPopup.h"
#include <QDialogButtonBox>
#include <QFileDialog>
#include <QGridLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "greucore/include/GuardedPtr.h"
#include "greumovies/include/Settings.h"
#include "greuui/include/Action.h"
#include "greuui/include/Icons.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

using namespace greu::core;
using namespace greu::ui;
using namespace greu::movies;

namespace greu::gaion::setup {

struct SettingsPopup::Detail {
    GuardedPtr<QGridLayout> layout;
    GuardedPtr<QLabel> basePathLabel, tilesPerLineBigLabel, tilesPerLineSmallLabel;
    GuardedPtr<QLineEdit> basePathEdit, tilesPerLineBigEdit, tilesPerLineSmallEdit;
    QIntValidator val{1, 100};
    GuardedPtr<Action> actSelectPath;
    GuardedPtr<QDialogButtonBox> buttons;
    GuardedPtr<QPushButton> ok, cancel;

    Detail() {
        actSelectPath = new Action(GREU_ICON(search), QObject::tr("Select path"));
        layout = new QGridLayout;
        uint row = 0;
        addNewWidget(basePathLabel, QObject::tr("Base path"), layout.get(), row, 0);
        addNewWidget(basePathEdit, layout.get(), row++, 1);
        addNewWidget(tilesPerLineBigLabel, QObject::tr("Tiles per line (big)"), layout.get(), row, 0);
        addNewWidget(tilesPerLineBigEdit, layout.get(), row++, 1);
        addNewWidget(tilesPerLineSmallLabel,
                  QObject::tr("Tiles per line (small)"),
                  layout.get(),
                  row,
                  0);
        addNewWidget(tilesPerLineSmallEdit, layout.get(), row++, 1);
        addNewWidget(buttons, layout.get(), row++, 0, 1, 2);
        ok = new QPushButton(GREU_ICON(ok), "");
        cancel = new QPushButton(GREU_ICON(cancel), "");
        buttons->addButton(ok.get(), QDialogButtonBox::AcceptRole);
        buttons->addButton(cancel.get(), QDialogButtonBox::RejectRole);
        tilesPerLineBigEdit->setValidator(&val);
        tilesPerLineSmallEdit->setValidator(&val);
        basePathEdit->addAction(actSelectPath.get(), QLineEdit::ActionPosition::TrailingPosition);
        basePathEdit->setText(Settings::instance()->pathPrefix());
        tilesPerLineBigEdit->setText(QString("%1").arg(Settings::instance()->tilesPerRowBig()));
        tilesPerLineSmallEdit->setText(QString("%1").arg(Settings::instance()->tilesPerRowSmall()));
        ui::connect(actSelectPath.get(), [this] {
            GuardedPtr<QFileDialog> fd;
            fd = new QFileDialog(nullptr,
                                 QObject::tr("Select path"),
                                 Settings::instance()->pathPrefix());
            fd->setFileMode(QFileDialog::Directory);
            if (fd->exec()) {
                auto files = fd->selectedFiles();
                if (files.size() != 1)
                    return;
                basePathEdit->setText(files[0]);
            }
        });
    }

    void save() {
        Settings::instance()->setPathPrefix(basePathEdit->text());
        Settings::instance()->setTilesPerRowBig(tilesPerLineBigEdit->text().toInt());
        Settings::instance()->setTilesPerRowSmall(tilesPerLineSmallEdit->text().toInt());
    }
};

SettingsPopup::SettingsPopup()
    : Dialog("SettingsPopup")
    , d(GREU_MKDET()) {
    setLayout(d->layout.get());
    ui::connect(d->ok.get(), [this] {
        d->save();
        accept();
    });
    ui::connect(d->cancel.get(), [this] { reject(); });
}

} // namespace greu::gaion::setup
