#include "LongRunnableDialog.h"
#include <QCloseEvent>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include "Dialog.h"
#include "Icons.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "utils.h"

using namespace boost::signals2;

namespace greu::ui {

using namespace core;

struct LongRunnableDialog::Detail {
    struct MyDialog : public ui::Dialog {
        QString const title;
        LongRunnable *const runnable;
        GuardedPtr<QPushButton> cancel;
        GuardedPtr<QVBoxLayout> layout;
        GuardedPtr<QProgressBar> progressBar;
        ScopedConnection connProgressed;

        MyDialog(QString const &title, LongRunnable *runnable)
            : title(title)
            , runnable(runnable) {}

        void setupUi() {
            setWindowTitle(title);
            layout = new QVBoxLayout;
            addNewWidget(progressBar, layout.get());
            addNewWidget(cancel, GREU_ICON(cancel), "", layout.get());
            setLayout(layout.get());
            runnable->progressed().connect([this](int p) { progressBar->setValue(p); });
            ui::connect(cancel.get(), [this] { runnable->cancel(); });
        }

        virtual void closeEvent(QCloseEvent *) override { runnable->cancel(); }
    };

    LongRunnable *const runnable;
    GuardedPtr<MyDialog> dialog;

    Detail(LongRunnable *runnable, QString const &title)
        : runnable(runnable)
        , dialog(new MyDialog(title, runnable)) {
        dialog->setupUi();
    }

    void start(LongRunnable::Worker *worker, bool noCancel) {
        ScopedConnection connFinish = runnable->finished().connect([this] { dialog->accept(); });
        runnable->start(worker, false);
        if (noCancel)
            dialog->cancel->setEnabled(false);
        dialog->exec();
        runnable->end();
    }
};

LongRunnableDialog::LongRunnableDialog(LongRunnable *runnable, QString const &title)
    : d(GREU_MKDET(runnable, title)) {}

void LongRunnableDialog::start(LongRunnable::Worker *worker, bool noCancel) {
    d->start(worker, noCancel);
}

} // namespace greu::ui
