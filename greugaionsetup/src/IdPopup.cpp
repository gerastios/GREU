#include "IdPopup.h"
#include <QDialogButtonBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include "Dispatcher.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/Settings.h"
#include "greucore/include/utils.h" // IWYU pragma: keep
#include "greuui/include/Icons.h"
#include "greuui/include/ImageContainer.h"
#include "greuui/include/connect.h"

namespace {
QString const movieIdKey{"movieId"};
QString const seriesIdKey{"seriesId"};
QString const seasonNumberKey{"seasonNumber"};
QString const episodeNumberKey{"episodeNumber"};

} // namespace

namespace greu::gaion::setup {

using namespace core;
using namespace ui;
using namespace movies;

struct IdPopup::Detail {
    int32_t id;
    uint seasonNumber, episodeNumber, variantSeasonNumber, variantEpisodeNumber;
    QString variantTitle;
    QFileInfo fileInfo;
    bool isMovie;
    GuardedPtr<QGridLayout> layout;
    GuardedPtr<QLabel> idLabel, seasonLabel, episodeLabel, variantSeasonLabel, variantEpisodeLabel,
        variantTitleLabel;
    GuardedPtr<QLineEdit> idEdit, seasonEdit, episodeEdit, variantSeasonEdit, variantEpisodeEdit,
        variantTitleEdit;
    GuardedPtr<QDialogButtonBox> toolBar;
    GuardedPtr<QPushButton> ok, cancel;
    QIntValidator val, valu;
    Settings stg;
    QImage img;

    Detail(QFileInfo const &fi, bool m)
        : fileInfo(fi)
        , isMovie(m)
        , stg("gaion") {}

    void setupUi(IdPopup *that) {
        val.setBottom(1);
        layout = new QGridLayout;
        idEdit = new QLineEdit;
        seasonEdit = new QLineEdit;
        seasonLabel = new QLabel(QObject::tr("Season number"));
        episodeEdit = new QLineEdit;
        episodeLabel = new QLabel(QObject::tr("Episode number"));
        variantSeasonEdit = new QLineEdit;
        variantSeasonLabel = new QLabel(QObject::tr("variant season number"));
        variantEpisodeEdit = new QLineEdit;
        variantEpisodeLabel = new QLabel(QObject::tr("variant episode number"));
        variantTitleEdit = new QLineEdit;
        variantTitleLabel = new QLabel(QObject::tr("variant title"));
        if (isMovie) {
            idLabel = new QLabel(QObject::tr("Movie id"));
            id = stg.value(movieIdKey).toInt();
            seasonEdit->setVisible(false);
            seasonLabel->setVisible(false);
            episodeEdit->setVisible(false);
            episodeLabel->setVisible(false);
            variantSeasonEdit->setVisible(false);
            variantSeasonLabel->setVisible(false);
            variantEpisodeEdit->setVisible(false);
            variantEpisodeLabel->setVisible(false);
        } else {
            idLabel = new QLabel(QObject::tr("Series id"));
            id = stg.value(seriesIdKey).toInt();
            seasonNumber = stg.value(seasonNumberKey).toUInt();
            episodeNumber = stg.value(episodeNumberKey).toUInt();
        }
        toolBar = new QDialogButtonBox;
        ok = new QPushButton(GREU_ICON(ok), "");
        cancel = new QPushButton(GREU_ICON(cancel), "");
        ui::connect(cancel.get(), [that] { that->reject(); });
        ui::connect(ok.get(), [this, that] {
            id = idEdit->text().toInt();
            seasonNumber = seasonEdit->text().toUInt();
            episodeNumber = episodeEdit->text().toUInt();
            variantSeasonNumber = variantSeasonEdit->text().toUInt();
            variantEpisodeNumber = variantEpisodeEdit->text().toUInt();
            variantTitle = variantTitleEdit->text();
            if (isMovie) {
                stg.setValue(movieIdKey, id);
            } else {
                stg.setValue(seriesIdKey, id);
                stg.setValue(seasonNumberKey, seasonNumber);
                stg.setValue(episodeNumberKey, episodeNumber);
            }
            that->accept();
        });
        toolBar->addButton(ok.get(), QDialogButtonBox::AcceptRole);
        toolBar->addButton(cancel.get(), QDialogButtonBox::RejectRole);
        int row = 0;
        layout->addWidget(idLabel.get(), row, 0);
        layout->addWidget(idEdit.get(), row++, 1);
        layout->addWidget(variantTitleLabel.get(), row, 0);
        layout->addWidget(variantTitleEdit.get(), row++, 1);
        layout->addWidget(seasonLabel.get(), row, 0);
        layout->addWidget(seasonEdit.get(), row++, 1);
        layout->addWidget(episodeLabel.get(), row, 0);
        layout->addWidget(episodeEdit.get(), row++, 1);
        layout->addWidget(variantSeasonLabel.get(), row, 0);
        layout->addWidget(variantSeasonEdit.get(), row++, 1);
        layout->addWidget(variantEpisodeLabel.get(), row, 0);
        layout->addWidget(variantEpisodeEdit.get(), row++, 1);
        layout->addWidget(toolBar.get(), row, 0);
        that->setLayout(layout.get());
        valu.setBottom(0);
        idEdit->setValidator(&val);
        seasonEdit->setValidator(&valu);
        episodeEdit->setValidator(&valu);
        variantSeasonEdit->setValidator(&valu);
        variantEpisodeEdit->setValidator(&valu);
        idEdit->setText(QString::number(id));
        seasonEdit->setText(QString::number(seasonNumber));
        episodeEdit->setText(QString::number(episodeNumber));
        that->setWindowTitle(QObject::tr("Assign file %1").arg(fileInfo.fileName()));
        if (isMovie)
            idEdit->setFocus();
        else
            episodeEdit->setFocus();
    }
};

IdPopup::IdPopup(QFileInfo const &fi, bool m)
    : Dialog("idPopup")
    , d(GREU_MKDET(fi, m)) {
    d->setupUi(this);
}

IdPopup::Id IdPopup::id() const {
    return {.id = d->id,
            .seasonNumber = d->seasonNumber,
            .episodeNumber = d->episodeNumber,
            .variantSeasonNumber = d->variantSeasonNumber,
            .variantEpisodeNumber = d->variantEpisodeNumber,
            .variantTitle = d->variantTitle};
}

} // namespace greu::gaion::setup
