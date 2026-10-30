#include "NormalizeTool.h"
#include <QCloseEvent>
#include <QFileInfo>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include "greucore/include/GuardedPtr.h"
#include "greutag/include/File.h"
#include "greuui/include/Icons.h"
#include "greuui/include/LineEdit.h"
#include "greuui/include/ProcessViewer.h"
#include "greuui/include/Settings.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"
#include <vector>

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

namespace {
QString const normalizeCommand("NormalizeCommand");
}

struct NormalizeTool::Detail {
    struct Runner {
        Detail *that;

        Runner(Detail *that)
            : that(that) {
            that->command->setEnabled(false);
            that->start->setEnabled(false);
            that->running = true;
        }

        void run() {
            auto words = that->command->text().split(" ");
            std::vector<QString> args;
            bool first = true;
            QString cmd;
            for (auto w : words) {
                if (first) {
                    cmd = w;
                    first = false;
                } else {
                    if (w == "%F") {
                        for (auto f : that->files) {
                            args.push_back(f->info().filePath());
                        }
                    } else {
                        args.push_back(w);
                    }
                }
            }
            that->processViewer->start(cmd, args);
        }

        ~Runner() {
            that->command->setEnabled(true);
            that->start->setEnabled(true);
            that->running = false;
        }
    };

    NormalizeTool *that;
    vector<File *> const &files;

    GuardedPtr<QVBoxLayout> mainLayout;
    GuardedPtr<QGridLayout> inputLayout;
    GuardedPtr<QLabel> commandLabel, comment;
    GuardedPtr<LineEdit> command;
    GuardedPtr<QPushButton> start;
    GuardedPtr<QWidget> processContainer;
    std::unique_ptr<ProcessViewer> processViewer;

    bool running = false;

    Detail(vector<tag::File *> const &files)
        : files(files) {}

    void setupUi() {
        mainLayout = new QVBoxLayout;
        that->setLayout(mainLayout.get());
        inputLayout = new QGridLayout;
        commandLabel = new QLabel(QObject::tr("Normalize command"));
        command = new LineEdit;
        commandLabel->setBuddy(command.get());
        command->setText(ui::Settings::instance().value(normalizeCommand, "").toString());
        comment = new QLabel(QObject::tr("%F - space separated file list"));
        start = new QPushButton(GREU_ICON(ok), QObject::tr("&Start"));
        ui::connect(start.get(), [this] {
            if (command->text().isEmpty()) {
                QMessageBox::information(that,
                                         QObject::tr("No command"),
                                         QObject::tr("Please enter a command to execute"));
                return;
            }
            Runner r(this);
            r.run();
        });
        inputLayout->addWidget(commandLabel.get(), 0, 0);
        inputLayout->addWidget(command.get(), 0, 1);
        inputLayout->addWidget(comment.get(), 1, 0, 1, 2);
        inputLayout->addWidget(start.get(), 2, 0, 1, 2);
        mainLayout->addLayout(inputLayout.get());
        addNewWidget(processContainer, mainLayout.get());
        processViewer = std::make_unique<ProcessViewer>(processContainer.get());

        ui::Settings::instance().restore(that);
    }

    ~Detail() {
        ui::Settings::instance().save(that);
        ui::Settings::instance().setValue(normalizeCommand, command->text());
    }

    void closeEvent(QCloseEvent *event) {
        if (running)
            event->ignore();
        else
            event->accept();
    }
};

NormalizeTool::NormalizeTool(vector<tag::File *> const &files)
    : d(GREU_MKDET(files)) {
    d->that = this;
    setObjectName("NormalizeDialog");
    d->setupUi();
}

void NormalizeTool::closeEvent(QCloseEvent *event) {
    d->closeEvent(event);
}

} // namespace greu::grylizo
