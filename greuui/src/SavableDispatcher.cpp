#include "SavableDispatcher.h"
#include <QMessageBox>
#include "LongRunnableDialog.h"
#include "MainWindow.h"
#include "greucore/include/Exception.h"
#include "greucore/include/Saver.h"
#include "greucore/include/utils.h" // IWYU pragma: keep

using namespace std;

namespace greu::ui {

using namespace core;

struct SavableDispatcher::Detail {
    bool noUiUpdate = false;

    bool save(bool all, SavableDispatcher *that, bool noCancel = false) {
        GREU_M->endEditing();
        if (!all) {
            auto cnt = that->modifiedCount();
            if (cnt == 0) {
                GREU_M->setStatusMessage(QObject::tr("Nothing to save"));
                return true;
            }
        }
        try {
            if (all)
                save(true, that->saver(), that, noCancel);
            else
                save(false, that->saver(), that, noCancel);
            GREU_M->setStatusMessage(QObject::tr("%1 objects saved").arg(that->savedCount()));
        } catch (core::Exception &e) {
            QMessageBox::critical(nullptr, "Error during saving", e.text(), QMessageBox::Ok);
            return false;
        }
        return true;
    }

    void save(bool all, Saver *saver, SavableDispatcher *that, bool noCancel = false) {
        noUiUpdate = true;
        saver->setSaveAll(all);
        if (saver->noDialog()) {
            saver->start(saver, true);
        } else {
            LongRunnableDialog dialog{saver, QObject::tr("Saving...")};
            dialog.start(saver, noCancel);
        }
        noUiUpdate = false;
        that->updateUi();
        auto e = saver->exception();
        if (e)
            std::rethrow_exception(e);
    }

    bool canClose(SavableDispatcher *that) {
        auto res = true;
        if (that->isModified()) {
            QMessageBox msgBox;
            msgBox.setText(QObject::tr("Some data was changed"));
            msgBox.setInformativeText(QObject::tr("Do you want to save your changes?"));
            msgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard
                                      | QMessageBox::Cancel);
            msgBox.setDefaultButton(QMessageBox::Save);
            switch (msgBox.exec()) {
            case QMessageBox::Save:
                res = save(false, that, false);
                break;
            case QMessageBox::Discard:
                break;
            case QMessageBox::Cancel:
                res = false;
                break;
            default:
                res = false;
            }
        }

        return res;
    }
};

SavableDispatcher::SavableDispatcher()
    : d(GREU_MKDET()) {}

void SavableDispatcher::saveModified(bool noCancel) {
    d->save(false, this, noCancel);
}

void SavableDispatcher::saveAll(bool noCancel) {
    QMessageBox msg;
    msg.setWindowTitle(QObject::tr("Save all"));
    msg.setText(QObject::tr("Do you want to save all data regardless whether they are changed? "
                            "This may take a very long time!"));
    msg.setStandardButtons({QMessageBox::Ok, QMessageBox::Cancel});
    auto answer = msg.exec();
    if (answer == QMessageBox::Ok)
        d->save(true, this, noCancel);
}

bool SavableDispatcher::canClose() {
    return d->canClose(this);
}

bool SavableDispatcher::noUiUpdate() const {
    return d->noUiUpdate;
}

} // namespace greu::ui
