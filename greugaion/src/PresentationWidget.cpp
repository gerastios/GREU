#include "PresentationWidget.h"
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QVBoxLayout>
#include "LinkController.h"
#include "MainWindow.h"
#include "NavigationController.h"
#include "PresentationItem.h"
#include "greucore/include/Uuid.h"
#include "greumovies/include/CreditHolder.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

namespace greu::gaion {

using namespace core;
using namespace movies;
using namespace ui;

struct PresentationWidget::Detail {
    LinkController linkController;
};

PresentationWidget::PresentationWidget()
    : d(GREU_MKDET()) {}

LinkController *PresentationWidget::linkController() const {
    return &d->linkController;
}

void PresentationWidget::gridEven(QGridLayout *layout,
                                  std::set<size_t> const &exclRows,
                                  std::set<size_t> const &exclCols) {
    for (int i = 0; i < layout->rowCount(); ++i) {
        if (exclRows.contains(i))
            layout->setRowStretch(i, 0);
        else
            layout->setRowStretch(i, 1);
    }
    for (int i = 0; i < layout->columnCount(); ++i) {
        if (exclCols.contains(i))
            layout->setColumnStretch(i, 0);
        else
            layout->setColumnStretch(i, 1);
    }
}

void PresentationWidget::gridLeft(QGridLayout *layout) {
    for (int i = 0; i < layout->columnCount() - 1; ++i)
        layout->setColumnStretch(i, 0);
    layout->setColumnStretch(layout->columnCount(), 1);
}

void PresentationWidget::boxEven(QBoxLayout *layout, std::set<size_t> const &excl) {
    for (int i = 0; i < layout->count(); ++i)
        if (excl.contains(i))
            layout->setStretch(i, 0);
        else
            layout->setStretch(i, 1);
}

PresentationWidget::CastCrewButton PresentationWidget::castCrewButton(CreditHolder const *holder,
                                                                      QVBoxLayout *layout) {
    CastCrewButton res;
    res.widget = new QWidget;
    res.layout = new QHBoxLayout;
    res.widget->setLayout(res.layout.get());
    layout->addWidget(res.widget.get());
    addNewWidget(res.button, QObject::tr("Cast && Crew"), res.layout.get());
    ui::connect(res.button.get(), [holder] {
        GREU_NAVC->setCurrent(Presentation{QObject::tr("Cast & Crew of %1").arg(holder->name()),
                                           ListType::AllCredits,
                                           {holder->uuid()}});
    });
    res.layout->addStretch(1);
    return res;
}

} // namespace greu::gaion
