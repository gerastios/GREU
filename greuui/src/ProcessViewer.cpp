#include "ProcessViewer.h"
#include <QEventLoop>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QRegularExpression>
#include <QScrollBar>
#include <QTextEdit>
#include <QVBoxLayout>
#include "Action.h"
#include "LineEdit.h"
#include "connect.h"
#include "greucore/include/GuardedPtr.h"
#include "utils.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct ProcessViewer::Detail {
    struct Runner {
        Detail *parent;

        Runner(Detail *parent)
            : parent(parent) {
            this->parent->enableRunWidgets(true);
            this->parent->input->setFocus();
        }

        void run(QProcess *process,
                 QString const &program,
                 vector<QString> const &arguments,
                 QIODevice::OpenMode mode) {
            process->start(program, ranges::to<QStringList>(arguments), mode);
        }

        ~Runner() { parent->enableRunWidgets(false); }
    };

    QWidget *widget;
    QProcess process;
    static constexpr char preHtml[] = "<html><style>body {background-color: black; color: "
                                      "chartreuse; font-family: monospace}</style><body>";
    static constexpr char postHtml[] = "</body></html>";

    GuardedPtr<QTextEdit> output, errorOutput;
    GuardedPtr<LineEdit> input;
    GuardedPtr<QLabel> labelActive, label;
    GuardedPtr<QPushButton> enter, send, kill;
    GuardedPtr<Action> actEnter, actSend;
    GuardedPtr<QVBoxLayout> layout;
    GuardedPtr<QGridLayout> grid;

    QString processOutput, processErrorOutput, error;

    QString path{};

    Detail(QWidget *widget)
        : widget(widget) {
        actEnter = new Action(QObject::tr("Enter"));
        actSend = new Action(QObject::tr("Send"));
        actEnter->setShortcuts({QKeySequence(Qt::Key_Enter), QKeySequence(Qt::Key_Return)});
        widget->addAction(actEnter.get());
        actSend->setShortcut(QKeySequence(int(Qt::CTRL) + Qt::Key_Enter));
        widget->addAction(actSend.get());
    }

    void setupUi() {
        layout = new QVBoxLayout();
        widget->setLayout(layout.get());
        addNewWidget(kill, QObject::tr("&Kill"), layout.get());
        ui::connect(kill.get(), [this] {
            if (process.state() != QProcess::Running)
                return;
            process.kill();
        });
        addNewWidget(labelActive, QObject::tr("No process"), layout.get());
        addNewWidget(output, layout.get());
        output->setReadOnly(true);
        addNewWidget(errorOutput, layout.get());
        errorOutput->setReadOnly(true);
        grid = new QGridLayout;
        layout->addLayout(grid.get());
        ui::connect(actEnter.get(), [this] { doEnter(); });
        ui::connect(actSend.get(), [this] { doSend(); });
        addNewWidget(label, QObject::tr("&Input"), grid.get(), 0, 0);
        addNewWidget(input, grid.get(), 0, 1);
        label->setBuddy(input.get());
        addNewWidget(enter, QObject::tr("&Enter"), grid.get(), 1, 0, 1, 2);
        ui::connect(enter.get(), [this] { doEnter(); });
        addNewWidget(send, QObject::tr("&Send"), grid.get(), 2, 0, 1, 2);
        ui::connect(send.get(), [this] { doSend(); });
        enableRunWidgets(false);
    }

    void start(QString const &program, vector<QString> const &arguments, QIODevice::OpenMode mode) {
        error = "";
        processOutput = "";
        processErrorOutput = "";
        setText();
        QEventLoop loop;
        if (!path.isEmpty())
            process.setWorkingDirectory(path);
        connectReadOutput(&process, [this] {
            auto out = QString(process.readAllStandardOutput());
            auto lines = out.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);
            for (auto l : lines) {
                if (!l.isEmpty()) {
                    if (processOutput.length() == 0)
                        processOutput = l;
                    else
                        processOutput += "<br>" + l;
                }
            }
            setText();
        });
        connectReadError(&process, [this] {
            auto out = QString(process.readAllStandardError());
            auto lines = out.split(QRegularExpression("[\r\n]"), Qt::SkipEmptyParts);
            for (auto l : lines) {
                if (!l.isEmpty()) {
                    if (processErrorOutput.length() == 0)
                        processErrorOutput = l;
                    else
                        processErrorOutput += "<br>" + l;
                }
            }
            setErrorText();
        });
        connectFinished(&process, [&loop] { loop.exit(); });
        connectStateChanged(&process, [this](QProcess::ProcessState state) {
            switch (state) {
            case QProcess::NotRunning:
                labelActive->setText(QObject::tr("Not running"));
                break;
            case QProcess::Starting:
                labelActive->setText(QObject::tr("Starting"));
                break;
            case QProcess::Running:
                labelActive->setText(QObject::tr("Running"));
                break;
            }
        });
        connectOnError(&process, [&loop] { loop.exit(); });
        {
            Runner r(this);
            r.run(&process, program, arguments, mode);
            loop.exec();
        }
        if (process.state() == QProcess::Running)
            process.kill();
        labelActive->setText(QObject::tr("Finished"));
    }

    ~Detail() {
        if (process.state() == QProcess::Running)
            process.kill();
    }

    void enableRunWidgets(bool enable) {
        input->setEnabled(enable);
        enter->setEnabled(enable);
        send->setEnabled(enable);
        kill->setEnabled(enable);
    }

    void doEnter() {
        if (!process.isWritable())
            return;
        if (process.state() != QProcess::Running)
            return;
        QString text = input->text() + "\n";
        process.write(text.toStdString().c_str());
        processOutput += input->text();
        input->clear();
        input->setFocus();
    }

    void doSend() {
        if (!process.isWritable())
            return;
        if (process.state() != QProcess::Running)
            return;
        process.write(input->text().toStdString().c_str());
        processOutput += input->text();
        input->clear();
        input->setFocus();
    }

    void setText() {
        output->setHtml(QString(preHtml) + processOutput + QString(postHtml));
        output->repaint();
        output->verticalScrollBar()->setValue(output->verticalScrollBar()->maximum());
        errorOutput->setHtml(QString(preHtml) + processErrorOutput + QString(postHtml));
        errorOutput->repaint();
        errorOutput->verticalScrollBar()->setValue(errorOutput->verticalScrollBar()->maximum());
    }

    void setErrorText() {
        errorOutput->setHtml(QString(preHtml) + processErrorOutput + QString(postHtml));
        errorOutput->repaint();
        errorOutput->verticalScrollBar()->setValue(errorOutput->verticalScrollBar()->maximum());
    }
};

ProcessViewer::ProcessViewer(QWidget *widget)
    : d(GREU_MKDET(widget)) {
    d->setupUi();
}

void ProcessViewer::setPath(QString const &v) {
    d->path = v;
}

void ProcessViewer::start(QString const &program,
                          vector<QString> const &arguments,
                          QIODevice::OpenMode mode) {
    d->start(program, arguments, mode);
}

} // namespace greu::ui
