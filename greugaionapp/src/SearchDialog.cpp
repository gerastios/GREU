#include "SearchDialog.h"
#include <QCheckBox>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QPushButton>
#include "greucore/include/GuardedPtr.h"
#include "greumovies/include/GenresDefinitionTable.h"
#include "greumovies/include/JobsTable.h"
#include "greumovies/include/SearchOptions.h"
#include "greuui/include/ComboBox.h"
#include "greuui/include/Icons.h"
#include "greuui/include/Settings.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

using namespace std;
using namespace greu::core;
using namespace greu::ui;
using namespace greu::movies;

namespace {
QString const genresKey{"searchGenres"};
QString const genreMovie1Key{"mv1"};
QString const genreMovie2Key{"mv2"};
QString const genreMovie3Key{"mv3"};
QString const genreSeries1Key{"tv1"};
QString const genreSeries2Key{"tv2"};
QString const genreSeries3Key{"tv3"};
QString const fieldsKey{"searchFields"};
QString const targetsKey{"searchTargets"};
QString const stringKey{"searchString"};
QString const stringPatternKey{"searchStringPattern"};
QString const departmentsKey{"searchDepartments"};
QString const department1Key{"dp1"};
QString const department2Key{"dp2"};
QString const department3Key{"dp3"};
} // namespace

namespace greu::gaion::app {

struct SearchDialog::Detail {
    struct Part {
        virtual ~Part() = default;
        virtual QWidget *widget() = 0;
    };

    struct SearchTargets : public Part {
        GuardedPtr<QGroupBox> _widget;
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<QCheckBox> movie, movieCollection, series, season, episode, collection, person;

        SearchTargets() {
            _widget = new QGroupBox(QObject::tr("Search objects"));
            layout = new QGridLayout;
            _widget->setLayout(layout.get());
            uint row = 0;
            addNewWidget(movie, QObject::tr("Movies"), layout.get(), row, 0);
            addNewWidget(movieCollection, QObject::tr("Movies collections"), layout.get(), row++, 1);
            addNewWidget(series, QObject::tr("Series"), layout.get(), row, 0);
            addNewWidget(season, QObject::tr("Seasons"), layout.get(), row++, 1);
            addNewWidget(episode, QObject::tr("Episodes"), layout.get(), row, 0);
            addNewWidget(collection, QObject::tr("Collections"), layout.get(), row++, 1);
            addNewWidget(person, QObject::tr("Persons"), layout.get(), row, 0);
            auto s = ui::Settings::instance().value(targetsKey, 0).toInt();
            movie->setChecked(s & SearchOptions::targetMovie);
            movieCollection->setChecked(s & SearchOptions::targetMovieCollection);
            series->setChecked(s & SearchOptions::targetSeries);
            season->setChecked(s & SearchOptions::targetSeason);
            episode->setChecked(s & SearchOptions::targetEpisode);
            collection->setChecked(s & SearchOptions::targetCollection);
            person->setChecked(s & SearchOptions::targetPerson);
        }

        ~SearchTargets() { ui::Settings::instance().setValue(targetsKey, (int) *this); }

        operator int() {
            return (movie->isChecked() ? SearchOptions::targetMovie : 0)
                   | (movieCollection->isChecked() ? SearchOptions::targetMovieCollection : 0)
                   | (series->isChecked() ? SearchOptions::targetSeries : 0)
                   | (season->isChecked() ? SearchOptions::targetSeason : 0)
                   | (episode->isChecked() ? SearchOptions::targetEpisode : 0)
                   | (collection->isChecked() ? SearchOptions::targetCollection : 0)
                   | (person->isChecked() ? SearchOptions::targetPerson : 0);
        }

        virtual QWidget *widget() override { return _widget.get(); }
    };

    struct SearchFields : public Part {
        GuardedPtr<QGroupBox> _widget;
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<QCheckBox> title, description;

        SearchFields() {
            _widget = new QGroupBox(QObject::tr("Search fields"));
            layout = new QGridLayout;
            _widget->setLayout(layout.get());
            addNewWidget(title, QObject::tr("Title/name"), layout.get(), 0, 0);
            addNewWidget(description, QObject::tr("Description"), layout.get(), 0, 1);
            auto s = ui::Settings::instance()
                         .value(fieldsKey,
                                SearchOptions::fieldTitle | SearchOptions::fieldDescription)
                         .toInt();
            title->setChecked(s & SearchOptions::fieldTitle);
            description->setChecked(s & SearchOptions::fieldDescription);
        }

        ~SearchFields() { ui::Settings::instance().setValue(fieldsKey, (int) *this); }

        operator int() {
            return (title->isChecked() ? SearchOptions::fieldTitle : 0)
                   | (description->isChecked() ? SearchOptions::fieldDescription : 0);
        }

        virtual QWidget *widget() override { return _widget.get(); }
    };

    struct Genres : public Part {
        GuardedPtr<QGroupBox> _widget;
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<QLabel> movie1Lbl, movie2Lbl, movie3Lbl, series1Lbl, series2Lbl, series3Lbl;
        GuardedPtr<ComboBox> movie1, movie2, movie3, series1, series2, series3;

        Genres() {
            _widget = new QGroupBox(QObject::tr("Only in genres..."));
            layout = new QGridLayout;
            _widget->setLayout(layout.get());
            int row = 0;
            addNewWidget(movie1Lbl, QObject::tr("Movie genre 1"), layout.get(), row, 0);
            addNewWidget(movie1, layout.get(), row++, 1);
            movie1->setEditable(false);
            addNewWidget(movie2Lbl, QObject::tr("Movie genre 2"), layout.get(), row, 0);
            addNewWidget(movie2, layout.get(), row++, 1);
            movie2->setEditable(false);
            addNewWidget(movie3Lbl, QObject::tr("Movie genre 3"), layout.get(), row, 0);
            addNewWidget(movie3, layout.get(), row++, 1);
            movie3->setEditable(false);
            addNewWidget(series1Lbl, QObject::tr("TV genre 1"), layout.get(), row, 0);
            addNewWidget(series1, layout.get(), row++, 1);
            series1->setEditable(false);
            addNewWidget(series2Lbl, QObject::tr("TV genre 2"), layout.get(), row, 0);
            addNewWidget(series2, layout.get(), row++, 1);
            series2->setEditable(false);
            addNewWidget(series3Lbl, QObject::tr("TV genre 3"), layout.get(), row, 0);
            addNewWidget(series3, layout.get(), row++, 1);
            series3->setEditable(false);
            auto movieGenres = GenresDefinitionTable::instance()->allMovieGenres();
            movieGenres.insert("");
            auto seriesGenres = GenresDefinitionTable::instance()->allSeriesGenres();
            seriesGenres.insert("");
            for (auto &g : movieGenres) {
                movie1->addItem(g);
                movie2->addItem(g);
                movie3->addItem(g);
            }
            for (auto &g : seriesGenres) {
                series1->addItem(g);
                series2->addItem(g);
                series3->addItem(g);
            }
            auto &s = ui::Settings::instance();
            s.beginGroup(genresKey);
            movie1->setCurrentText(s.value(genreMovie1Key, "").toString());
            movie2->setCurrentText(s.value(genreMovie2Key, "").toString());
            movie3->setCurrentText(s.value(genreMovie3Key, "").toString());
            series1->setCurrentText(s.value(genreSeries1Key, "").toString());
            series2->setCurrentText(s.value(genreSeries2Key, "").toString());
            series3->setCurrentText(s.value(genreSeries3Key, "").toString());
            s.endGroup();
        }

        ~Genres() {
            auto &s = ui::Settings::instance();
            s.beginGroup(genresKey);
            s.setValue(genreMovie1Key, movie1->currentText());
            s.setValue(genreMovie2Key, movie2->currentText());
            s.setValue(genreMovie3Key, movie3->currentText());
            s.setValue(genreSeries1Key, series1->currentText());
            s.setValue(genreSeries2Key, series2->currentText());
            s.setValue(genreSeries3Key, series3->currentText());
            s.endGroup();
        }

        vector<QString> movieGenres() {
            decltype(movieGenres()) res;
            if (!movie1->currentText().isEmpty())
                res.push_back(movie1->currentText());
            if (!movie2->currentText().isEmpty())
                res.push_back(movie2->currentText());
            if (!movie3->currentText().isEmpty())
                res.push_back(movie3->currentText());
            return res;
        }

        vector<QString> seriesGenres() {
            decltype(seriesGenres()) res;
            if (!series1->currentText().isEmpty())
                res.push_back(series1->currentText());
            if (!series2->currentText().isEmpty())
                res.push_back(series2->currentText());
            if (!series3->currentText().isEmpty())
                res.push_back(series3->currentText());
            return res;
        }

        virtual QWidget *widget() override { return _widget.get(); }
    };

    struct Departments : public Part {
        GuardedPtr<QGroupBox> _widget;
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<QLabel> label1, label2, label3;
        GuardedPtr<ComboBox> dep1, dep2, dep3;

        Departments() {
            _widget = new QGroupBox(QObject::tr("Only in job departments..."));
            layout = new QGridLayout;
            _widget->setLayout(layout.get());
            int row = 0;
            addNewWidget(label1, QObject::tr("Department 1"), layout.get(), row, 0);
            addNewWidget(dep1, layout.get(), row++, 1);
            dep1->setEditable(false);
            addNewWidget(label2, QObject::tr("Department 2"), layout.get(), row, 0);
            addNewWidget(dep2, layout.get(), row++, 1);
            dep2->setEditable(false);
            addNewWidget(label3, QObject::tr("Department 3"), layout.get(), row, 0);
            addNewWidget(dep3, layout.get(), row++, 1);
            dep3->setEditable(false);
            auto departments = JobsTable::instance()->allDepartments();
            departments.insert("");
            for (auto &g : departments) {
                dep1->addItem(g);
                dep2->addItem(g);
                dep3->addItem(g);
            }
            auto &s = ui::Settings::instance();
            s.beginGroup(departmentsKey);
            dep1->setCurrentText(s.value(department1Key, "").toString());
            dep2->setCurrentText(s.value(department2Key, "").toString());
            dep3->setCurrentText(s.value(department3Key, "").toString());
            s.endGroup();
        }

        ~Departments() {
            auto &s = ui::Settings::instance();
            s.beginGroup(departmentsKey);
            s.setValue(department1Key, dep1->currentText());
            s.setValue(department2Key, dep2->currentText());
            s.setValue(department3Key, dep3->currentText());
            s.endGroup();
        }

        operator vector<QString>() {
            decltype(vector<QString>()) res;
            if (!dep1->currentText().isEmpty())
                res.push_back(dep1->currentText());
            if (!dep2->currentText().isEmpty())
                res.push_back(dep2->currentText());
            if (!dep3->currentText().isEmpty())
                res.push_back(dep3->currentText());
            return res;
        }

        virtual QWidget *widget() override { return _widget.get(); }
    };

    struct SearchField : public Part {
        GuardedPtr<QWidget> _widget;
        GuardedPtr<QGridLayout> layout;
        GuardedPtr<QLabel> label;
        GuardedPtr<QLineEdit> edit;
        GuardedPtr<QCheckBox> pattern;

        SearchField() {
            _widget = new QWidget;
            layout = new QGridLayout;
            _widget->setLayout(layout.get());
            addNewWidget(label,
                         QObject::tr("Search string (use % and _ as wildcards)"),
                         layout.get(),
                         0,
                         0);
            addNewWidget(edit, layout.get(), 0, 1);
            edit->setText(ui::Settings::instance().value(stringKey, "").toString());
            addNewWidget(pattern,
                         QObject::tr("is pattern (wildcards % and _, ignore case)"),
                         layout.get(),
                         1,
                         0,
                         1,
                         2);
            pattern->setChecked(ui::Settings::instance().value(stringPatternKey, false).toBool());
        }

        ~SearchField() {
            ui::Settings::instance().setValue(stringKey, edit->text());
            ui::Settings::instance().setValue(stringPatternKey, pattern->isChecked());
        }

        virtual QWidget *widget() override { return _widget.get(); }
    };

    GuardedPtr<QGridLayout> layout;
    SearchTargets targets;
    SearchFields fields;
    Genres genres;
    Departments departments;
    SearchField search;
    GuardedPtr<QDialogButtonBox> buttons;
    GuardedPtr<QPushButton> ok, cancel;

    Detail() {
        layout = new QGridLayout;
        layout->addWidget(targets.widget(), 0, 0);
        layout->addWidget(fields.widget(), 0, 1);
        layout->addWidget(genres.widget(), 1, 0);
        layout->addWidget(departments.widget(), 1, 1);
        layout->addWidget(search.widget(), 2, 0, 1, 2);
        addNewWidget(buttons, layout.get(), 3, 0, 1, 2);
        ok = new QPushButton(GREU_ICON(ok), "");
        cancel = new QPushButton(GREU_ICON(cancel), "");
        buttons->addButton(ok.get(), QDialogButtonBox::AcceptRole);
        buttons->addButton(cancel.get(), QDialogButtonBox::RejectRole);
    }
};

SearchDialog::SearchDialog()
    : Dialog("SearchDialog")
    , d(GREU_MKDET()) {
    setLayout(d->layout.get());
    ui::connect(d->ok.get(), [this] {
        if (d->search.edit->text().isEmpty()) {
            QMessageBox::critical(this,
                                  QObject::tr("Error"),
                                  QObject::tr("Please enter a search string!"));
            return;
        }
        if ((int) d->fields == 0) {
            QMessageBox::critical(this,
                                  QObject::tr("Error"),
                                  QObject::tr("Title or description must be checked!"));
            return;
        }
        if ((int) d->targets == 0) {
            QMessageBox::critical(this,
                                  QObject::tr("Error"),
                                  QObject::tr("At least one target must be checked!"));
            return;
        }
        accept();
    });
    ui::connect(d->cancel.get(), [this] { reject(); });
}

SearchOptions SearchDialog::options() const {
    return {
        .target = d->targets,
        .field = d->fields,
        .movieGenres = d->genres.movieGenres(),
        .seriesGenres = d->genres.seriesGenres(),
        .departments = d->departments,
        .searchString = d->search.edit->text(),
        .isPattern = d->search.pattern->isChecked(),
    };
}

} // namespace greu::gaion::app
