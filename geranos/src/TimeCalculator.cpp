#include "TimeCalculator.h"
#include <QApplication>
#include <QClipboard>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/stringUtils.h"
#include "greuui/include/Icons.h"
#include "greuui/include/LineEdit.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

namespace greu::geranos {

using namespace core;
using namespace ui;

struct TimeCalculator::Detail {
    GuardedPtr<QGridLayout> mainLayout;
    GuardedPtr<QHBoxLayout> buttonLayout;
    GuardedPtr<QPushButton> plus, minus, toMillisecs, toTime;
    GuardedPtr<QLabel> labelA;
    GuardedPtr<LineEdit> valueA;
    GuardedPtr<QLabel> labelB;
    GuardedPtr<LineEdit> valueB;
    GuardedPtr<QLabel> labelResult;
    GuardedPtr<LineEdit> result;

    constexpr static auto const add = [](uint a, uint b) { return a + b; };
    constexpr static auto const sub = [](uint a, uint b) { return a > b ? a - b : b - a; };

    Detail() {
        buttonLayout = new QHBoxLayout;
        addNewWidget(plus, GREU_ICON(add), "", buttonLayout.get());
        addNewWidget(minus, GREU_ICON(subtract), "", buttonLayout.get());
        addNewWidget(toMillisecs, QObject::tr("milliseconds"), buttonLayout.get());
        addNewWidget(toTime, QObject::tr("time"), buttonLayout.get());
        plus->setShortcut(Qt::CTRL | Qt::Key_Plus);
        minus->setShortcut(Qt::CTRL | Qt::Key_Minus);
        mainLayout = new QGridLayout;
        addNewWidget(labelA, QObject::tr("value A"), mainLayout.get(), 0, 0);
        addNewWidget(valueA, mainLayout.get(), 0, 1, 1, 4);
        addNewWidget(labelB, QObject::tr("value B"), mainLayout.get(), 1, 0);
        addNewWidget(valueB, mainLayout.get(), 1, 1, 1, 4);
        addNewWidget(labelResult, QObject::tr("result"), mainLayout.get(), 2, 0);
        addNewWidget(result, mainLayout.get(), 2, 1, 1, 4);
        labelA->setBuddy(valueA.get());
        labelB->setBuddy(valueB.get());
        labelResult->setBuddy(result.get());
        result->setReadOnly(true);
        mainLayout->addLayout(buttonLayout.get(), 3, 0, 1, 5);

        ui::connect(plus.get(), [this] { computeResult(add); });

        ui::connect(minus.get(), [this] { computeResult(sub); });

        ui::connect(toMillisecs.get(), [this] {
            uint a = 0;
            if (!valueA->text().simplified().isEmpty()) {
                a = computeMillisecondsFromTimeString(valueA->text().simplified());
            }
            result->setText(QString::number(a));
        });

        ui::connect(toTime.get(), [this] {
            result->setText(computeTimeStringFromMilliseconds(valueA->text().simplified().toUInt()));
        });
    }

    template<typename T>
    void computeResult(T func) {
        uint a = 0;
        uint b = 0;
        if (!valueA->text().simplified().isEmpty()) {
            a = computeMillisecondsFromTimeString(valueA->text().simplified());
        }
        if (!valueB->text().simplified().isEmpty()) {
            b = computeMillisecondsFromTimeString(valueB->text().simplified());
        }
        result->setText(computeTimeStringFromMilliseconds(func(a, b)));
        QApplication::clipboard()->setText(result->text());
    }
};

TimeCalculator::TimeCalculator(Qt::WindowFlags f)
    : Dialog(f)
    , d(GREU_MKDET()) {
    setLayout(d->mainLayout.get());
}

} // namespace greu::geranos
