#include "CollectionEditor.h"
#include <QButtonGroup>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QTextEdit>
#include "ImageImporter.h"
#include "greucore/include/GuardedPtr.h"
#include "greugaion/include/Styles.h"
#include "greuui/include/Icons.h"
#include "greuui/include/ImageContainer.h"
#include "greuui/include/ImageLabel.h"
#include "greuui/include/connect.h"
#include "greuui/include/imageTransformation.h"

using namespace greu::ui;
using namespace greu::core;
using namespace greu::movies;

namespace greu::gaion::setup {

struct CollectionEditor::Detail {
    GuardedPtr<QGridLayout> layout;
    GuardedPtr<QLabel> textLabel, descriptionLabel;
    GuardedPtr<QLineEdit> textEdit;
    GuardedPtr<QTextEdit> descriptionEdit;
    GuardedPtr<ImageLabel> image;
    GuardedPtr<QButtonGroup> sortButtons;
    GuardedPtr<QRadioButton> sortByText, sortByDate;
    GuardedPtr<QDialogButtonBox> buttons;
    GuardedPtr<QPushButton> ok, cancel, setImage;
    ImageContainer img;

    void setupUi(CollectionEditor *that) {
        layout = new QGridLayout;
        textLabel = new QLabel(QObject::tr("Text"));
        textEdit = new QLineEdit;
        descriptionLabel = new QLabel(QObject::tr("Description"));
        descriptionEdit = new QTextEdit;
        image = new ImageLabel;
        buttons = new QDialogButtonBox;
        ok = new QPushButton(GREU_ICON(ok), "");
        cancel = new QPushButton(GREU_ICON(cancel), "");
        setImage = new QPushButton(GREU_ICON(image), "");
        buttons->addButton(ok.get(), QDialogButtonBox::AcceptRole);
        buttons->addButton(cancel.get(), QDialogButtonBox::RejectRole);
        buttons->addButton(setImage.get(), QDialogButtonBox::ActionRole);
        ui::connect(ok.get(), [that] { that->accept(); });
        ui::connect(cancel.get(), [that] { that->reject(); });
        ui::connect(setImage.get(), [this, that] {
            img = ImageImporter::import();
            auto w = GREU_STYL->width() / 8;
            auto h = w * 1.5;
            if (img.isValid()) {
                image->setImage(scale(img.toImage(),
                                      w,
                                      h,
                                      *that,
                                      ImageOrientation::portrait,
                                      Qt::FastTransformation));
                image->showImage();
            }
        });
        sortButtons = new QButtonGroup;
        sortByText = new QRadioButton(QObject::tr("Sort by text"));
        sortByDate = new QRadioButton(QObject::tr("Sort by date"));
        sortByText->setChecked(true);
        sortButtons->addButton(sortByText.get());
        sortButtons->addButton(sortByDate.get());
        layout->addWidget(textLabel.get(), 0, 0);
        layout->addWidget(textEdit.get(), 0, 1);
        layout->addWidget(descriptionLabel.get(), 1, 0);
        layout->addWidget(descriptionEdit.get(), 1, 1, 4, 5);
        layout->addWidget(sortByText.get(), 5, 0, 1, 6);
        layout->addWidget(sortByDate.get(), 6, 0, 1, 6);
        layout->addWidget(image.get(), 7, 0, 10, 6);
        layout->addWidget(buttons.get(), 17, 0, 1, 6);
    }
};

CollectionEditor::CollectionEditor()
    : d(GREU_MKDET()) {
    d->setupUi(this);
    setLayout(d->layout.get());
}

QString CollectionEditor::text() const {
    return d->textEdit->text();
}

QString CollectionEditor::description() const {
    return d->descriptionEdit->toPlainText();
}

Collection::SortOrder CollectionEditor::sortOrder() const {
    if (d->sortByText->isChecked())
        return Collection::byText;
    return Collection::byDate;
}
ImageContainer CollectionEditor::image() const {
    return d->img;
}

} // namespace greu::gaion::setup
