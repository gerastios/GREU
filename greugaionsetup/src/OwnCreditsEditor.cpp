#include "OwnCreditsEditor.h"
#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QPushButton>
#include <QTextEdit>
#include "ImageImporter.h"
#include "greucore/include/GuardedPtr.h"
#include "greugaion/include/Styles.h"
#include "greumovies/include/JobsTable.h"
#include "greuui/include/Action.h"
#include "greuui/include/ComboBox.h"
#include "greuui/include/DateEdit.h"
#include "greuui/include/Icons.h"
#include "greuui/include/ImageLabel.h"
#include "greuui/include/TableView.h"
#include "greuui/include/connect.h"

using namespace std;

namespace greu::gaion::setup {

using namespace core;
using namespace ui;
using namespace movies;

struct OwnCreditsEditor::Detail {
    struct Model : public QAbstractTableModel {
        void update() {
            layoutAboutToBeChanged();
            layoutChanged();
        }
    };

    struct CastModel : public Model {
        OwnMovie const &ownMovie;

#define A_TMDB_ID 0
#define A_NAME 1
#define A_CHARACTER 2
#define A_ORDER 3

        CastModel(OwnMovie const &ow)
            : ownMovie(ow) {}

        virtual int rowCount(QModelIndex const &) const override { return ownMovie.cast.size(); }

        virtual int columnCount(QModelIndex const &) const override { return A_ORDER + 1; }

        virtual QVariant data(QModelIndex const &index, int role) const override {
            if (role != Qt::DisplayRole || index.row() >= (int) ownMovie.cast.size())
                return {};
            auto &c = ownMovie.cast[index.row()];
            switch (index.column()) {
            case A_TMDB_ID:
                return c.person.tmdbId;
            case A_NAME:
                return c.person.name;
            case A_CHARACTER:
                return c.character;
            case A_ORDER:
                return c.order;
            }
            return {};
        }
    };

    struct CrewModel : public Model {
        OwnMovie const &ownMovie;

#define R_TMDB_ID 0
#define R_NAME 1
#define R_JOB 2

        CrewModel(OwnMovie const &ow)
            : ownMovie(ow) {}

        virtual int rowCount(QModelIndex const &) const override { return ownMovie.crew.size(); }

        virtual int columnCount(QModelIndex const &) const override { return R_JOB + 1; }

        virtual QVariant data(QModelIndex const &index, int role) const override {
            if (role != Qt::DisplayRole || index.row() >= (int) ownMovie.crew.size())
                return {};
            auto &c = ownMovie.crew[index.row()];
            switch (index.column()) {
            case R_TMDB_ID:
                return c.person.tmdbId;
            case R_NAME:
                return c.person.name;
            case R_JOB:
                return c.job;
            }
            return {};
        }
    };

    OwnMovie ownMovie;
    OwnCreditsEditor::Type type;
    GuardedPtr<QGridLayout> layout;
    GuardedPtr<TableView> creditsTable;
    unique_ptr<Model> creditsModel;
    GuardedPtr<QLabel> tmdbIdLabel, nameLabel, jobLabel, characterLabel, orderLabel, biographyLabel,
        birthDateLabel, deathDateLabel;
    GuardedPtr<QLineEdit> tmdbId, name, character, order;
    GuardedPtr<ComboBox> job;
    GuardedPtr<QTextEdit> biography;
    GuardedPtr<DateEdit> birthDate, deathDate;
    GuardedPtr<ImageLabel> imageLabel;
    ImageContainer img;
    GuardedPtr<QDialogButtonBox> buttons;
    GuardedPtr<QPushButton> ok, cancel, setImage, addCredit;
    QIntValidator val;
    Action actDelete{"Delete line"};

    Detail(OwnMovie const &om, Type t)
        : ownMovie(om)
        , type(t) {}

    void setupUi(OwnCreditsEditor *that) {
        layout = new QGridLayout;
        int row = 0;
        creditsTable = new TableView("owncreditstable");
        creditsTable->addContextAction(&actDelete, [this](int row, auto, auto) {
            if (type == OwnCreditsEditor::cast) {
                ownMovie.cast.erase(ownMovie.cast.begin() + row);
            } else {
                ownMovie.crew.erase(ownMovie.crew.begin() + row);
            }
        });
        if (type == OwnCreditsEditor::cast)
            creditsModel = make_unique<CastModel>(ownMovie);
        else
            creditsModel = make_unique<CrewModel>(ownMovie);
        creditsTable->setModel(creditsModel.get());
        tmdbIdLabel = new QLabel(QObject::tr("TMDB person id"));
        nameLabel = new QLabel(QObject::tr("Person name"));
        jobLabel = new QLabel(QObject::tr("Job"));
        characterLabel = new QLabel(QObject::tr("Character"));
        orderLabel = new QLabel(QObject::tr("Order"));
        biographyLabel = new QLabel(QObject::tr("Biography"));
        birthDateLabel = new QLabel(QObject::tr("Birth date"));
        deathDateLabel = new QLabel(QObject::tr("Death date"));
        tmdbId = new QLineEdit;
        tmdbId->setValidator(&val);
        name = new QLineEdit;
        job = new ComboBox;
        job->setEditable(false);
        auto jobs = JobsTable::instance()->allJobs();
        for (auto &j : jobs)
            job->addItem(j);
        character = new QLineEdit;
        order = new QLineEdit;
        order->setValidator(&val);
        biography = new QTextEdit;
        birthDate = new DateEdit;
        birthDate->setDisplayFormat("dd.MM.yyyy");
        deathDate = new DateEdit;
        deathDate->setDisplayFormat("dd.MM.yyyy");
        imageLabel = new ImageLabel;
        buttons = new QDialogButtonBox;
        ok = new QPushButton(GREU_ICON(ok), "");
        cancel = new QPushButton(GREU_ICON(cancel), "");
        setImage = new QPushButton(QObject::tr("Set image"));
        addCredit = new QPushButton(QObject::tr("Add credit"));
        buttons->addButton(ok.get(), QDialogButtonBox::AcceptRole);
        buttons->addButton(cancel.get(), QDialogButtonBox::RejectRole);
        buttons->addButton(setImage.get(), QDialogButtonBox::ActionRole);
        buttons->addButton(addCredit.get(), QDialogButtonBox::ActionRole);
        layout->addWidget(creditsTable.get(), row, 0, 10, 2);
        row += 10;
        layout->addWidget(tmdbIdLabel.get(), row, 0);
        layout->addWidget(tmdbId.get(), row++, 1);
        layout->addWidget(nameLabel.get(), row, 0);
        layout->addWidget(name.get(), row++, 1);
        layout->addWidget(jobLabel.get(), row, 0);
        layout->addWidget(job.get(), row++, 1);
        layout->addWidget(characterLabel.get(), row, 0);
        layout->addWidget(character.get(), row++, 1);
        layout->addWidget(orderLabel.get(), row, 0);
        layout->addWidget(order.get(), row++, 1);
        layout->addWidget(biographyLabel.get(), row, 0);
        layout->addWidget(biography.get(), row, 1, 3, 1);
        row += 3;
        layout->addWidget(birthDateLabel.get(), row, 0);
        layout->addWidget(birthDate.get(), row++, 1);
        layout->addWidget(deathDateLabel.get(), row, 0);
        layout->addWidget(deathDate.get(), row++, 1);
        layout->addWidget(imageLabel.get(), row, 0, 5, 2);
        row += 5;
        layout->addWidget(buttons.get(), row++, 0, 1, 2);
        if (type == OwnCreditsEditor::cast) {
            jobLabel->setVisible(false);
            job->setVisible(false);
        } else {
            characterLabel->setVisible(false);
            character->setVisible(false);
            orderLabel->setVisible(false);
            order->setVisible(false);
        }
        ui::connect(ok.get(), [that] { that->accept(); });
        ui::connect(cancel.get(), [that] { that->reject(); });
        ui::connect(addCredit.get(), [this] {
            OwnPerson person = {.tmdbId = tmdbId->text().toInt(),
                                .name = name->text(),
                                .biography = biography->toPlainText(),
                                .birthDate = birthDate->value(),
                                .deathDate = deathDate->value(),
                                .image = img};
            if (type == OwnCreditsEditor::cast) {
                ownMovie.cast.push_back(OwnCast{.person = person,
                                                .character = character->text(),
                                                .order = order->text().toInt()});
            } else {
                ownMovie.crew.push_back(OwnCrew{.person = person, .job = job->currentText()});
            }
            creditsModel->update();
            tmdbId->clear();
            name->clear();
            biography->clear();
            character->clear();
            order->clear();
            birthDate->setValue({});
            deathDate->setValue({});
            imageLabel->setImage({});
            img = {};
        });
        ui::connect(setImage.get(), [this] {
            auto img = ImageImporter::import();
            if (img.isValid()) {
                imageLabel->setImage(img.toImage().scaledToHeight(GREU_STYL->height() / 8));
                imageLabel->showImage();
            }
        });
    }
};

OwnCreditsEditor::OwnCreditsEditor(OwnMovie const &om, Type t)
    : d(GREU_MKDET(om, t)) {
    d->setupUi(this);
    setLayout(d->layout.get());
}

OwnMovie OwnCreditsEditor::ownMovie() const {
    return d->ownMovie;
}

} // namespace greu::gaion::setup
