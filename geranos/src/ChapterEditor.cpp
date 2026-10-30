#include "ChapterEditor.h"
#include <QCheckBox>
#include <QClipboard>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QProcess>
#include <QPushButton>
#include <QScrollArea>
#include "Application.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/stringUtils.h"
#include "greuui/include/Icons.h"
#include "greuui/include/LineEdit.h"
#include "greuui/include/Settings.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"
#include <vector>

using namespace std;

namespace greu::geranos {

using namespace ui;
using namespace core;

struct ChapterEditor::Detail {
    struct ChapterLine {
        GuardedPtr<LineEdit> time, title;
        GuardedPtr<QLabel> numberLabel;
        uint nr;

        ChapterLine(uint number, QGridLayout *layout) {
            nr = number - 1;
            numberLabel = new QLabel(QString::number(number - 1));
            addNewWidget(numberLabel, QString::number(number - 1), layout, number - 1, 0);
            addNewWidget(time, layout, number - 1, 1);
            addNewWidget(title, layout, number - 1, 2);
            title->setMinimumWidth(280);
        }
    };

    QString filePath;
    vector<ChapterLine> chapters;
    GuardedPtr<QCheckBox> isStartTime;
    GuardedPtr<QPushButton> ok, cancel, insertTimes, insertTitles;
    GuardedPtr<QScrollArea> scroll;
    GuardedPtr<QWidget> viewPort;
    GuardedPtr<QVBoxLayout> scrollLayout;
    GuardedPtr<QHBoxLayout> buttonsLayout;
    GuardedPtr<QGridLayout> mainLayout;

    Detail(QString const &filePath)
        : filePath(filePath) {}

    void setupUi(ChapterEditor *that) {
        scroll = new QScrollArea;
        viewPort = new QWidget;
        scrollLayout = new QVBoxLayout;
        mainLayout = new QGridLayout;
        isStartTime = new QCheckBox(QObject::tr("start times"));
        isStartTime->setChecked(true);
        connectState(isStartTime.get(), [this](auto state) {
            insertTimes->setText(state == Qt::Checked ? QObject::tr("Starts") : QObject::tr("Ends"));
        });

        buttonsLayout = new QHBoxLayout;
        ok = new QPushButton(GREU_ICON(ok), "");
        ui::connect(ok.get(), [that] { that->accept(); });
        ok->setDefault(true);
        cancel = new QPushButton(GREU_ICON(cancel), "");
        ui::connect(cancel.get(), [that] { that->reject(); });
        insertTimes = new QPushButton(GREU_ICON(import), QObject::tr("Starts"));
        ui::connect(insertTimes.get(), [this] {
            auto text = Application::clipboard()->text();
            if (!text.isEmpty()) {
                auto words = text.split("\n", Qt::KeepEmptyParts);
                for (uint i = 0; i < words.size() && i < chapters.size(); i++) {
                    chapters[i].time->setText(words[i]);
                }
            }
        });
        insertTitles = new QPushButton(GREU_ICON(import), QObject::tr("Titles"));
        ui::connect(insertTitles.get(), [this] {
            auto text = Application::clipboard()->text();
            if (!text.isEmpty()) {
                auto words = text.split("\n", Qt::KeepEmptyParts);
                for (uint i = 0; i < words.size() && i < chapters.size(); i++) {
                    chapters[i].title->setText(words[i]);
                }
            }
        });
        buttonsLayout->addWidget(ok.get());
        buttonsLayout->addWidget(cancel.get());
        buttonsLayout->addWidget(insertTimes.get());
        buttonsLayout->addWidget(insertTitles.get());
        scrollLayout->addLayout(buttonsLayout.get());

        mainLayout->addWidget(isStartTime.get(), 0, 0);
        for (uint i = 1; i <= 60; i++) {
            chapters.push_back(ChapterLine{i + 1, mainLayout.get()});
        }

        viewPort->setLayout(mainLayout.get());
        scroll->setWidget(viewPort.get()); // important this is done here!
        scrollLayout->addWidget(scroll.get());

        ui::Settings::instance().restore(that);

        that->setWindowTitle(filePath);
        that->setLayout(scrollLayout.get());
    }

    QString _chapters() {
        struct Time {
            int start, end;
            QString title;
        };

        QFileInfo fi{filePath};
        QProcess p;
        p.setWorkingDirectory(fi.path());
        vector<QString> args{"-hide_banner", fi.fileName()};
        p.start("/usr/bin/ffprobe", ranges::to<QStringList>(args));
        p.waitForFinished(-1);
        auto out = p.readAllStandardError();
        uint max = 72000000;
        auto found = out.indexOf("Duration: ");
        if (found >= 0) {
            found += 10;
            QString maxtime;
            while (found < out.length() && out.at(found) != QChar(',')
                   && out.at(found) != QChar(' ')) {
                maxtime += out.at(found++);
            }
            if (!maxtime.isEmpty()) {
                max = computeMillisecondsFromTimeString(maxtime);
            }
        }

        QString res;
        vector<Time> times;
        for (auto &c : chapters) {
            if (!c.time->text().simplified().isEmpty()) {
                Time t{.start = -1, .end = -1, .title = c.title->text()};
                if (isStartTime->checkState() == Qt::Checked) {
                    t.start = computeMillisecondsFromTimeString(c.time->text().simplified());
                } else {
                    t.end = computeMillisecondsFromTimeString(c.time->text().simplified());
                }
                times.push_back(t);
            }
        }
        for (uint i = 0; i < times.size(); i++) {
            if (i == 0 && times[0].start == -1) {
                res += QString("[CHAPTER]\n") + "TIMEBASE=1/1000\n" + "START=0\n" + "END="
                       + QString::number(times[0].end) + "\n" + "TITLE=" + times[0].title + "\n";
            } else if (i == times.size() - 1 && times[i].end == -1) {
                if (times[i].start == -1) {
                    res += QString("[CHAPTER]\n") + "TIMEBASE=1/1000\n"
                           + "START=" + QString::number(times[i - 1].end) + "\n" + "END="
                           + QString::number(max) + "\n" + "TITLE=" + times[i].title + "\n";
                } else {
                    res += QString("[CHAPTER]\n") + "TIMEBASE=1/1000\n" + "START="
                           + QString::number(times[i].start) + "\n" + "END=" + QString::number(max)
                           + "\n" + "TITLE=" + times[i].title + "\n";
                }
            } else if (times[i].start == -1) {
                res += QString("[CHAPTER]\n") + "TIMEBASE=1/1000\n"
                       + "START=" + QString::number(times[i - 1].end) + "\n" + "END="
                       + QString::number(times[i].end) + "\n" + "TITLE=" + times[i].title + "\n";
            } else if (times[i].end == -1) {
                res += QString("[CHAPTER]\n") + "TIMEBASE=1/1000\n"
                       + "START=" + QString::number(times[i].start) + "\n"
                       + "END=" + QString::number(times[i + 1].start) + "\n"
                       + "TITLE=" + times[i].title + "\n";
            } else {
                res += QString("[CHAPTER]\n") + "TIMEBASE=1/1000\n"
                       + "START=" + QString::number(times[i].start) + "\n" + "END="
                       + QString::number(times[i].end) + "\n" + "TITLE=" + times[i].title + "\n";
            }
        }
        return res;
    }
};

ChapterEditor::ChapterEditor(QString const &filePath)
    : Dialog("ChapterEditor")
    , d(GREU_MKDET(filePath)) {
    d->setupUi(this);
}

ChapterEditor::~ChapterEditor() {
    ui::Settings::instance().save(this);
}

QString ChapterEditor::chapters() const {
    return d->_chapters();
}

} // namespace greu::geranos
