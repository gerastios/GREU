#include "HeadInfoWidget.h"
#include <QBoxLayout>
#include <QLabel>
#include <QScrollArea>
#include "Dispatcher.h"
#include "FeatureModel.h"
#include "LinkController.h"
#include "MovieObjectItem.h"
#include "MovieObjectWidget.h"
#include "PlayableFileWidgets.h"
#include "Styles.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/utils.h"  // IWYU pragma: keep
#include "greumovies/include/File.h" // IWYU pragma: keep
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::gaion {

using namespace core;
using namespace ui;

struct HeadInfoWidget::Detail {
    GuardedPtr<QWidget> widget;
    GuardedPtr<QVBoxLayout> layout;
    GuardedPtr<QLabel> title, originaleTitle, releaseGenresRuntime, tagline, born, died,
        knownForDepartment, overview, belongsTo;

    void setup(PresentationWidget *parent,
               FeatureModel const *model,
               bool addLink,
               QString const &typeString,
               bool addSpecific) {
        widget = new QWidget;
        layout = new QVBoxLayout;
        layout->setAlignment(Qt::AlignTop);
        widget->setLayout(layout.get());
        buildTitle(parent, model, addLink, typeString);
        buildOriginalTitle(model);
        buildReleaseGenresRuntime(model);
        buildTagline(model);
        buildPersonDuration(model);
        buildKnownForDepartment(model);
        buildDescription(model);
        buildBelongsTo(parent, model);
        if (addSpecific) {
            auto mo = dynamic_cast<MovieObjectWidget *>(parent);
            if (mo)
                mo->addAtHeadInfoEnd(layout.get());
        }
    }

    void buildTitle(auto const parent, auto model, bool addLink, auto const &typeString) {
        auto releaseDate = model->releaseDate();
        QString titleAddition;
        if (releaseDate.isValid() && !typeString.isEmpty())
            titleAddition = QString("[%1, %2]").arg(releaseDate.toString("yyyy")).arg(typeString);
        else if (releaseDate.isValid())
            titleAddition = QString("[%1]").arg(releaseDate.toString("yyyy"));
        else if (!typeString.isEmpty())
            titleAddition = QString("[%1]").arg(typeString);
        if (addLink && model->linkedUuid()) {
            title = new QLabel(
                QString("%1 %2")
                    .arg(GREU_STYL->titleLink(parent->linkController()->buildLink(
                                                  Presentation(model->linkedUuid().value())),
                                              model->title()))
                    .arg(GREU_STYL->titleAddition(titleAddition)));
            ui::connect(title.get(), [](QString const &s) { GREU_D->linkClicked(s); });
        } else {
            title = new QLabel(QString("%1 %2")
                                   .arg(GREU_STYL->title(model->title()))
                                   .arg(GREU_STYL->titleAddition(titleAddition)));
        }
        layout->addWidget(title.get());
    }

    void buildOriginalTitle(auto model) {
        auto o = model->originalTitle();
        if (!(o.isEmpty() || o == model->title())) {
            addNewWidget(originaleTitle,
                         GREU_STYL->originalTitle(QString("[%1]").arg(o)),
                         layout.get());
        }
    }

    void buildTagline(auto model) {
        auto t = model->tagline();
        if (!t.isEmpty())
            addNewWidget(tagline, GREU_STYL->tagline(t), layout.get());
    }

    void buildReleaseGenresRuntime(auto model) {
        auto releaseDate = model->releaseDate();
        auto file = model->file();
        auto genres = model->genres();
        vector<QString> strings;
        if (releaseDate.isValid())
            strings.push_back(releaseDate.toString("dd.MM.yyyy"));
        if (file)
            strings.push_back(runTime(file->runtimeSecs()));
        if (!genres.empty())
            strings.push_back(ranges::to<QString>((genres | views::join_with(QString(" ⬪ ")))));
        releaseGenresRuntime = new QLabel(GREU_STYL->genresRuntime(
            ranges::to<QString>((strings | views::join_with(QString(" • "))))));
        layout->addWidget(releaseGenresRuntime.get());
    }

    void buildPersonDuration(auto model) {
        auto b = model->born();
        auto p = model->bornPlace();
        auto d = model->died();
        auto a = model->age();
        if (b.isValid() && d.isValid() && !p.isEmpty())
            addNewWidget(born,
                         GREU_STYL->tagline(
                             QObject::tr("born at %1 in %2").arg(b.toString("d.M.yyyy")).arg(p)),
                         layout.get());
        else if (b.isValid() && d.isValid() && p.isEmpty())
            addNewWidget(born,
                         GREU_STYL->tagline(QObject::tr("born at %12").arg(b.toString("d.M.yyyy"))),
                         layout.get());
        else if (b.isValid() && !p.isEmpty())
            addNewWidget(born,
                         GREU_STYL->tagline(QObject::tr("born at %1 in %2 (age %3)")
                                                .arg(b.toString("d.M.yyyy"))
                                                .arg(p)
                                                .arg(QString::number(a))),
                         layout.get());
        else if (b.isValid())
            addNewWidget(born,
                         GREU_STYL->tagline(
                             QObject::tr("born at %1 (age %2)")
                                 .arg(b.toString("d.M.yyyy"))
                                 .arg(a == 0 ? QObject::tr("unknown age") : QString::number(a))),
                         layout.get());
        if (d.isValid())
            addNewWidget(died,
                         GREU_STYL->tagline(
                             QObject::tr("died at %1 (at the age of %2)")
                                 .arg(d.toString("d.M.yyyy"))
                                 .arg(a == 0 ? QObject::tr("unknown age") : QString::number(a))),
                         layout.get());
    }

    void buildKnownForDepartment(auto model) {
        auto o = model->knownForDepartment();
        if (!o.isEmpty())
            addNewWidget(knownForDepartment,
                         GREU_STYL->tagline(QObject::tr("known for department: %1").arg(o)),
                         layout.get());
    }

    void buildDescription(auto model) {
        auto o = model->description();
        if (!o.isEmpty()) {
            addNewWidget(overview, GREU_STYL->overview(o), layout.get());
            overview->setWordWrap(true);
        }
    }

    void buildBelongsTo(auto const parent, auto model) {
        auto b = model->belongsTo();
        if (b.size() > 0) {
            addNewWidget(belongsTo,
                         GREU_STYL->collectionLabel(QObject::tr("belongs to: "))
                             + buildBelongsToString(parent, b),
                         layout.get());
            // ui::connect(belongsTo.get(), [](QString const &s) { GREU_D->linkClicked(s); });
        }
    }

    QString buildBelongsToString(auto const parent, FeatureModel::BelongGroups const &groups) {
        QString res;
        auto fg = true;
        for (auto const &g : groups) {
            if (!fg) {
                res += ";";
            }
            fg = false;
            res += GREU_STYL->collectionLabel(QString(" %1 ").arg(g.type));
            auto f = true;
            for (auto const &b : g.entries) {
                if (!f) {
                    res += ", ";
                }
                f = false;
                res += GREU_STYL->collection(parent->linkController()->buildLink(
                                                 Presentation{b.uuid}),
                                             b.title);
            }
        }
        return res;
    }
};

HeadInfoWidget::HeadInfoWidget(
    PresentationWidget *p, FeatureModel const *m, bool al, QString const &ts, bool as)
    : d(GREU_MKDET()) {
    d->setup(p, m, al, ts, as);
}

QWidget *HeadInfoWidget::widget() const {
    return d->widget.get();
}

} // namespace greu::gaion
