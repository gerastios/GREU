#include "OwnMovieEditor.h"
#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMimeData>
#include <QPushButton>
#include <QTextEdit>
#include "OwnCreditsEditor.h"
#include "OwnGenresEditor.h"
#include "greucore/include/GuardedPtr.h"
#include "greugaion/include/Styles.h"
#include "greumovies/include/OwnMovie.h"
#include "greuui/include/DateEdit.h"
#include "greuui/include/Icons.h"
#include "greuui/include/ImageContainer.h"
#include "greuui/include/ImageLabel.h"
#include "greuui/include/ImageTool.h"
#include "greuui/include/connect.h"

using namespace std;
using namespace greu::ui;
using namespace greu::core;
using namespace greu::movies;

namespace greu::gaion::setup {

struct OwnMovieEditor::Detail {
    GuardedPtr<QGridLayout> layout;
    GuardedPtr<QLabel> titleLabel, taglineLabel, originalTitleLabel, originalLanguageCodeLabel,
        descriptionLabel, releaseDateLabel;
    GuardedPtr<QLineEdit> title, tagline, originalTitle, originalLanguageCode;
    GuardedPtr<DateEdit> releaseDate;
    GuardedPtr<QTextEdit> description;
    GuardedPtr<ImageLabel> imageLabel;
    GuardedPtr<QDialogButtonBox> buttons;
    GuardedPtr<QPushButton> ok, cancel, setImage, addCast, addCrew, addGenre;

    OwnMovie ownMovie;

    void setupUi(OwnMovieEditor *that) {
        layout = new QGridLayout;
        titleLabel = new QLabel(QObject::tr("Title"));
        taglineLabel = new QLabel(QObject::tr("Tagline"));
        originalTitleLabel = new QLabel(QObject::tr("Original title"));
        descriptionLabel = new QLabel(QObject::tr("Description"));
        releaseDateLabel = new QLabel(QObject::tr("Release date"));
        originalLanguageCodeLabel = new QLabel(QObject::tr("Original language code"));
        title = new QLineEdit;
        tagline = new QLineEdit;
        originalTitle = new QLineEdit;
        releaseDate = new DateEdit;
        releaseDate->setDisplayFormat("dd.MM.yyyy");
        description = new QTextEdit;
        originalLanguageCode = new QLineEdit;
        imageLabel = new ImageLabel;
        buttons = new QDialogButtonBox;
        ok = new QPushButton(GREU_ICON(ok), "");
        cancel = new QPushButton(GREU_ICON(cancel), "");
        setImage = new QPushButton(QObject::tr("upload poster from clipboard"));
        addCast = new QPushButton(QObject::tr("Add cast"));
        addCrew = new QPushButton(QObject::tr("Add crew"));
        addGenre = new QPushButton(QObject::tr("Add genre"));
        buttons->addButton(ok.get(), QDialogButtonBox::ButtonRole::AcceptRole);
        buttons->addButton(cancel.get(), QDialogButtonBox::ButtonRole::RejectRole);
        buttons->addButton(setImage.get(), QDialogButtonBox::ButtonRole::ActionRole);
        buttons->addButton(addCast.get(), QDialogButtonBox::ButtonRole::ActionRole);
        buttons->addButton(addCrew.get(), QDialogButtonBox::ButtonRole::ActionRole);
        buttons->addButton(addGenre.get(), QDialogButtonBox::ButtonRole::ActionRole);
        auto row = 0;
        layout->addWidget(titleLabel.get(), row, 0);
        layout->addWidget(title.get(), row++, 1);
        layout->addWidget(originalTitleLabel.get(), row, 0);
        layout->addWidget(originalTitle.get(), row++, 1);
        layout->addWidget(originalLanguageCodeLabel.get(), row, 0);
        layout->addWidget(originalLanguageCode.get(), row++, 1);
        layout->addWidget(taglineLabel.get(), row, 0);
        layout->addWidget(tagline.get(), row++, 1);
        layout->addWidget(releaseDateLabel.get(), row, 0);
        layout->addWidget(releaseDate.get(), row++, 1);
        layout->addWidget(descriptionLabel.get(), row, 0);
        layout->addWidget(description.get(), row++, 1, 3, 1);
        layout->addWidget(imageLabel.get(), row, 0, 4, 2);
        row += 4;
        layout->addWidget(buttons.get(), row++, 0, 1, 2);
        ui::connect(ok.get(), [that] { that->accept(); });
        ui::connect(cancel.get(), [that] { that->reject(); });
        ui::connect(setImage.get(), [this] {
            auto cl = QApplication::clipboard();
            cl->ownsClipboard();
            if (!cl->mimeData()->hasImage())
                return;
            auto image = cl->image();
            if (image.isNull())
                return;
            GuardedPtr<ImageTool> tool;
            tool = new ImageTool(image, QObject::tr("Movie poster"), 0);
            if (tool->exec()) {
                image = tool->image();
                ownMovie.image = ImageContainer::fromImage(image);
                imageLabel->setImage(
                    ownMovie.image.toImage().scaledToHeight(GREU_STYL->height() / 16));
                imageLabel->showImage();
            }
        });
        ui::connect(addCast.get(), [this] {
            GuardedPtr<OwnCreditsEditor> cred = new OwnCreditsEditor(ownMovie,
                                                                     OwnCreditsEditor::cast);
            if (cred->exec())
                ownMovie = cred->ownMovie();
        });
        ui::connect(addCrew.get(), [this] {
            GuardedPtr<OwnCreditsEditor> cred = new OwnCreditsEditor(ownMovie,
                                                                     OwnCreditsEditor::crew);
            if (cred->exec())
                ownMovie = cred->ownMovie();
        });
        ui::connect(addGenre.get(), [this] {
            GuardedPtr<OwnGenresEditor> ged = new OwnGenresEditor(ownMovie);
            if (ged->exec())
                ownMovie = ged->ownMovie();
        });
    }

    void setOwnMovie(OwnMovie const &om) {
        ownMovie = om;
        title->setText(ownMovie.title);
        tagline->setText(ownMovie.tagline);
        originalTitle->setText(ownMovie.originalTitle);
        description->setPlainText(ownMovie.description);
        releaseDate->setValue(ownMovie.releaseDate);
        imageLabel->setImage(ownMovie.image.toImage().scaledToWidth(GREU_STYL->width() / 8));
    }

    void updateMovie() {
        ownMovie.title = title->text();
        ownMovie.tagline = tagline->text();
        ownMovie.originalTitle = originalTitle->text();
        ownMovie.description = description->toPlainText();
        ownMovie.releaseDate = releaseDate->value();
    }
};

OwnMovieEditor::OwnMovieEditor()
    : Dialog("ownmovieeditor")
    , d(GREU_MKDET()) {
    d->setupUi(this);
    setLayout(d->layout.get());
}

void OwnMovieEditor::setOwnMovie(OwnMovie const &) {}

OwnMovie const &OwnMovieEditor::ownMovie() const {
    d->updateMovie();
    return d->ownMovie;
}

} // namespace greu::gaion::setup
