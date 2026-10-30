#include "CreditWidget.h"
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include "Dispatcher.h"
#include "LinkController.h"
#include "Presentation.h"
#include "PresentationWidget.h"
#include "Styles.h"
#include "greucore/include/GuardedPtr.h"
#include "greumovies/include/Person.h"
#include "greuui/include/ImageButton.h"
#include "greuui/include/connect.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace movies;
using namespace ui;

struct CreditWidget::Detail {
    GuardedPtr<QGroupBox> widget;
    GuardedPtr<QGridLayout> layout;
    struct Entry {
        GuardedPtr<QWidget> widget;
        GuardedPtr<QVBoxLayout> layout;
        GuardedPtr<ImageButton> image;
        GuardedPtr<QLabel> person, job;
        ScopedConnection connImage;
    };
    vector<unique_ptr<Entry>> entries;

    Detail(QString const &title,
           MediumCredits const &credits,
           ListType type,
           size_t entriesPerRow,
           PresentationWidget *parent) {
        widget = new QGroupBox(title);
        widget->setStyleSheet(
            QString("QGroupBox { font-weight: bold; font-size: %1px; }")
                .arg((int) GREU_STYL->width() / 109));
        layout = new QGridLayout;
        widget->setLayout(layout.get());
        size_t row = 0;
        size_t column = 0;
        for (auto const &c : credits) {
            auto e = make_unique<Entry>();
            e->widget = new QWidget;
            e->layout = new QVBoxLayout;
            e->widget->setLayout(e->layout.get());
            if (type == ListType::Pictures) {
                e->image = new ImageButton();
                e->image->setImage(c.person->image()
                                       .scaledToWidth(GREU_STYL->width() / entriesPerRow / 1.3,
                                                      Qt::TransformationMode::SmoothTransformation),
                                   parent->linkController()->buildLink(
                                       Presentation(c.person->uuid())));
                e->layout->addWidget(e->image.get());
                e->connImage = e->image->clicked().connect(
                    [](QString const &s) { GREU_D->linkClicked(s); });
            }
            e->job = new QLabel(GREU_STYL->job(c.job));
            e->job->setWordWrap(true);
            e->person = new QLabel(GREU_STYL->jobLink(parent->linkController()->buildLink(
                                                          Presentation(c.person->uuid())),
                                                      c.person->name()));
            e->layout->addWidget(e->person.get());
            ui::connect(e->person.get(), [](QString const &s) { GREU_D->linkClicked(s); });
            e->layout->addWidget(e->job.get());
            layout->addWidget(e->widget.get(), row, column++);
            if (column == entriesPerRow) {
                column = 0;
                ++row;
            }
            entries.push_back(std::move(e));
        }
        layout->setColumnStretch(layout->columnCount() - 1, 1);
    }
};

CreditWidget::CreditWidget(QString const &title,
                           movies::MediumCredits const &credits,
                           ListType listType,
                           size_t columnsPerRow,
                           PresentationWidget *widget)
    : d(GREU_MKDET(title, credits, listType, columnsPerRow, widget)) {}

QWidget *CreditWidget::widget() const {
    return d->widget.get();
}

} // namespace greu::gaion
