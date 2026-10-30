#include "ConvertDialog.h"
#include <QCheckBox>
#include <QClipboard>
#include <QGridLayout>
#include <QGroupBox>
#include <QIntValidator>
#include <QLabel>
#include <QMessageBox>
#include <QScrollArea>
#include <QToolBar>
#include <QVBoxLayout>
#include "Application.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/Language.h"
#include "greucore/include/Settings.h"
#include "greucore/include/Time.h"
#include "greucore/include/hexUtils.h"
#include "greuui/include/Action.h"
#include "greuui/include/ComboBox.h"
#include "greuui/include/Icons.h"
#include "greuui/include/LineEdit.h"
#include "greuui/include/Notepad.h"
#include "greuui/include/Settings.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"
#include <vector>

using namespace std;

namespace greu::geranos {

using namespace core;
using namespace ui;

struct ConvertDialog::Detail {
    struct TrackEditor {
        GuardedPtr<QLabel> trackNumber, trackId;
        GuardedPtr<LineEdit> copiedTrackNumber;
        GuardedPtr<QLabel> duration, codecName, channels, channelLayout;
        GuardedPtr<ComboBox> language, title, audioCodec;
        uint id;
    };

    struct TrackGroup {
        GuardedPtr<QGroupBox> box;
        GuardedPtr<QGridLayout> layout;
        vector<TrackEditor> editors;
        QIntValidator validator;

        TrackGroup(QString const &title,
                   QVBoxLayout *mainLayout,
                   Media::Tracks const &tracks,
                   core::Settings &trackSettings,
                   vector<Language const *> const &languages,
                   QString const &settingsGroup = "") {
            validator.setBottom(0);
            validator.setTop(400);
            addNewWidget(box, title, mainLayout);
            layout = new QGridLayout;
            box->setLayout(layout.get());
            uint row = 0;
            for (auto &t : tracks) {
                TrackEditor te{.trackNumber = new QLabel(QString::number(t.number)),
                               .trackId = new QLabel(convertToHexString(t.id)),
                               .copiedTrackNumber = new LineEdit,
                               .duration = new QLabel(
                                   Time::fromSeconds(t.duration).toString("hh:mm:ss")),
                               .codecName = new QLabel(t.codecName),
                               .channels = t.type == Media::Audio
                                               ? new QLabel(QString::number(t.channels))
                                               : new QLabel,
                               .channelLayout = t.type == Media::Audio ? new QLabel(t.channelLayout)
                                                                       : new QLabel,
                               .language = new ComboBox,
                               .title = new ComboBox,
                               .audioCodec = new ComboBox,
                               .id = t.id};
                te.copiedTrackNumber->setValidator(&validator);
                if (t.type == Media::Video) {
                    te.copiedTrackNumber->setText(QString::number(t.number));
                    te.copiedTrackNumber->setEnabled(false);
                }
                if (t.type != Media::Audio) {
                    te.channels->setEnabled(false);
                    te.channelLayout->setEnabled(false);
                    te.audioCodec->setEnabled(false);
                }
                te.audioCodec->addItems({"", "ac3", "aac", "dts", "pcm"});
                te.language->addItem(QString{});
                int index = 1;
                for (auto &lang : languages) {
                    te.language->addItem(lang->name());
                    auto ol = Language::fromCode2(t.languageCode2);
                    if (ol && *ol.value() == *lang) {
                        te.language->setCurrentIndex(index);
                    }
                    ++index;
                };
                te.title->addItems(
                    {{""}, {"5.1"}, {"Commentary"}, {"DTS"}, {"Kommentar"}, {"Mono"}, {"Stereo"}});
                auto col = 0;
                layout->addWidget(te.trackNumber.get(), row, col++);
                layout->addWidget(te.copiedTrackNumber.get(), row, col++);
                layout->addWidget(te.trackId.get(), row, col++);
                layout->addWidget(te.language.get(), row, col++);
                layout->addWidget(te.title.get(), row, col++);
                layout->addWidget(te.duration.get(), row, col++);
                layout->addWidget(te.codecName.get(), row, col++);
                layout->addWidget(te.channels.get(), row, col++);
                layout->addWidget(te.audioCodec.get(), row, col++);
                if (t.type == Media::Video) {
                    te.title->setEnabled(false);
                    te.language->setEnabled(false);
                }
                te.title->setEditable(true);
                te.copiedTrackNumber->setMaximumWidth(40);
                if (!settingsGroup.isEmpty()) {
                    trackSettings.beginGroup(settingsGroup + "_" + QString::number(t.id));
                    te.copiedTrackNumber->setText(trackSettings.value("copied", "").toString());
                    te.language->setCurrentText(trackSettings.value("language", "").toString());
                    te.title->setCurrentText(trackSettings.value("title", "").toString());
                    trackSettings.endGroup();
                }
                ++row;
                editors.push_back(std::move(te));
            }
        }
    };

    Media media;
    GuardedPtr<QScrollArea> scroll;
    GuardedPtr<QToolBar> toolbar;
    GuardedPtr<Action> actConvert, actCancel, actSelAllAudio, actUnselAllAudio, actSelAllSubtitle,
        actUnselAllSubtitle, actCopy, actError;
    GuardedPtr<QGroupBox> viewPort;
    GuardedPtr<QVBoxLayout> mainLayout, scrollLayout;
    GuardedPtr<QGridLayout> optionsLayout;
    GuardedPtr<QLabel> useVobLabel;
    GuardedPtr<QCheckBox> useVob;
    GuardedPtr<LineEdit> fromTime;
    GuardedPtr<QLabel> fromLabel;
    GuardedPtr<LineEdit> toTime;
    GuardedPtr<QLabel> toLabel;
    GuardedPtr<Notepad> errorDialog;
    unique_ptr<TrackGroup> videoTracks, audioTracks, subtitleTracks;
    core::Settings trackSettings;
    vector<Language const *> languages;

    Detail(Media media)
        : media(std::move(media))
        , trackSettings(Application::instance()->applicationName()) {
        languages.push_back(Language::fromCode2(Language::Code2::Arabic).value());
        languages.push_back(Language::fromCode2(Language::Code2::Catalan).value());
        languages.push_back(Language::fromCode2(Language::Code2::Chinese).value());
        languages.push_back(Language::fromCode2(Language::Code2::Czech).value());
        languages.push_back(Language::fromCode2(Language::Code2::Danish).value());
        languages.push_back(Language::fromCode2(Language::Code2::Dutch).value());
        languages.push_back(Language::fromCode2(Language::Code2::English).value());
        languages.push_back(Language::fromCode2(Language::Code2::Finnish).value());
        languages.push_back(Language::fromCode2(Language::Code2::French).value());
        languages.push_back(Language::fromCode2(Language::Code2::German).value());
        languages.push_back(Language::fromCode2(Language::Code2::GreekModern).value());
        languages.push_back(Language::fromCode2(Language::Code2::Hungarian).value());
        languages.push_back(Language::fromCode2(Language::Code2::Italian).value());
        languages.push_back(Language::fromCode2(Language::Code2::Japanese).value());
        languages.push_back(Language::fromCode2(Language::Code2::Norwegian).value());
        languages.push_back(Language::fromCode2(Language::Code2::Polish).value());
        languages.push_back(Language::fromCode2(Language::Code2::Portuguese).value());
        languages.push_back(Language::fromCode2(Language::Code2::Russian).value());
        languages.push_back(Language::fromCode2(Language::Code2::Slovak).value());
        languages.push_back(Language::fromCode2(Language::Code2::Slovenian).value());
        languages.push_back(Language::fromCode2(Language::Code2::Spanish).value());
        languages.push_back(Language::fromCode2(Language::Code2::Swedish).value());
        languages.push_back(Language::fromCode2(Language::Code2::Turkish).value());
    }

    void setupUi(ConvertDialog *that) {
        mainLayout = new QVBoxLayout;

        actConvert = new Action(Icons::icon(Icons::convert), QString{});
        actCancel = new Action(Icons::icon(Icons::cancel), QString{});
        actSelAllAudio = new Action(Icons::icon(Icons::select_all), QObject::tr("audio tracks"));
        actUnselAllAudio = new Action(Icons::icon(Icons::select_none), QObject::tr("audio tracks"));
        actSelAllSubtitle = new Action(Icons::icon(Icons::select_all),
                                       QObject::tr("subtitle tracks"));
        actUnselAllSubtitle = new Action(Icons::icon(Icons::select_none),
                                         QObject::tr("subtitle tracks"));
        actCopy = new Action(Icons::icon(Icons::copy), QObject::tr("copy command"));
        actError = new Action(Icons::icon(Icons::error), QObject::tr("show Error"));

        scroll = new QScrollArea;
        scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);

        viewPort = new QGroupBox;

        toolbar = new QToolBar;
        toolbar->addAction(actConvert.get());
        toolbar->addAction(actCancel.get());
        toolbar->addSeparator();
        toolbar->addAction(actSelAllAudio.get());
        toolbar->addAction(actUnselAllAudio.get());
        toolbar->addAction(actSelAllSubtitle.get());
        toolbar->addAction(actUnselAllSubtitle.get());
        toolbar->addAction(actCopy.get());
        toolbar->addAction(actError.get());
        toolbar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
        mainLayout->addWidget(toolbar.get());

        ui::connect(actCancel.get(), [that] { that->reject(); });
        ui::connect(actConvert.get(), [this, that] {
            media.convert(conversion(audioTracks.get()),
                          conversion(subtitleTracks.get()),
                          fromTime->text().simplified(),
                          toTime->text().simplified(),
                          useVob->isChecked());
            if (media.hasError())
                QMessageBox::critical(that, QObject::tr("Conversion failed"), media.errorText());
            saveTracksSettings();
        });
        ui::connect(actSelAllAudio.get(), [this] { selectTracks(audioTracks->editors, true); });
        ui::connect(actUnselAllAudio.get(), [this] { selectTracks(audioTracks->editors, false); });
        ui::connect(actSelAllSubtitle.get(),
                    [this] { selectTracks(subtitleTracks->editors, true); });
        ui::connect(actUnselAllSubtitle.get(),
                    [this] { selectTracks(subtitleTracks->editors, false); });
        ui::connect(actCopy.get(),
                    [this] { QApplication::clipboard()->setText(media.lastCommand()); });
        ui::connect(actError.get(), [this] {
            if (errorDialog)
                errorDialog->hide();
            errorDialog = new Notepad("errorWindow");
            errorDialog->setupUi();
            errorDialog->setText(media.errorText());
            errorDialog->show();
            errorDialog->raise();
            errorDialog->activateWindow();
        });

        scrollLayout = new QVBoxLayout;
        optionsLayout = new QGridLayout;
        addNewWidget(useVob, optionsLayout.get(), 0, 0);
        addNewWidget(useVobLabel, QObject::tr("use VOB instead of MP4"), optionsLayout.get(), 0, 1);
        useVobLabel->setBuddy(useVob.get());
        fromTime = new LineEdit;
        fromLabel = new QLabel(QObject::tr("from"));
        fromLabel->setBuddy(fromTime.get());
        toTime = new LineEdit;
        toLabel = new QLabel(QObject::tr("to"));
        toLabel->setBuddy(toTime.get());
        addNewWidget(fromLabel, QObject::tr("from"), optionsLayout.get(), 1, 0);
        addNewWidget(fromTime, optionsLayout.get(), 1, 1);
        addNewWidget(toLabel, QObject::tr("to"), optionsLayout.get(), 1, 2);
        addNewWidget(toTime, optionsLayout.get(), 1, 3);
        mainLayout->addLayout(optionsLayout.get());
        videoTracks = make_unique<TrackGroup>(QObject::tr("Video tracks"),
                                              scrollLayout.get(),
                                              media.videoTracks(),
                                              trackSettings,
                                              languages);
        audioTracks = make_unique<TrackGroup>(QObject::tr("Audio tracks"),
                                              scrollLayout.get(),
                                              media.audioTracks(),
                                              trackSettings,
                                              languages,
                                              "Audio");
        subtitleTracks = make_unique<TrackGroup>(QObject::tr("Subtitle tracks"),
                                                 scrollLayout.get(),
                                                 media.subtitleTracks(),
                                                 trackSettings,
                                                 languages,
                                                 "Subtitle");

        viewPort->setLayout(scrollLayout.get());
        scroll->setWidget(viewPort.get()); // important this is done here!
        mainLayout->addWidget(scroll.get());

        ui::Settings::instance().restore(that);

        that->setWindowTitle(media.fileName());
        that->setLayout(mainLayout.get());
    }

    static void selectTracks(vector<TrackEditor> &tracks, bool select) {
        for (auto &t : tracks) {
            if (select) {
                if (t.copiedTrackNumber->text().isEmpty())
                    t.copiedTrackNumber->setText(t.trackNumber->text());
            } else {
                t.copiedTrackNumber->setText("");
                t.title->setCurrentText("");
                t.language->setCurrentText("");
            }
        }
    }

    Media::TrackConversions conversion(TrackGroup const *tracks) const {
        decltype(Detail::conversion(nullptr)) res;
        for (auto &t : tracks->editors) {
            if (t.copiedTrackNumber->text().isEmpty())
                continue;
            Media::TrackConversion c{
                .numberOld = t.trackNumber->text().toUInt(),
                .numberNew = t.copiedTrackNumber->text().toUInt(),
                .language = t.language->currentText(),
                .title = t.title->currentText(),
                .audioCodec = t.audioCodec->currentText() == "dts"   ? Media::Dts
                              : t.audioCodec->currentText() == "ac3" ? Media::Ac3
                              : t.audioCodec->currentText() == "aac" ? Media::Aac
                              : t.audioCodec->currentText() == "pcm" ? Media::Pcm
                                                                     : Media::Default,
            };
            if (c.numberNew == 0)
                continue;
            res.push_back(c);
        }
        sort(res.begin(), res.end(), [](auto &a, auto &b) { return a.numberNew < b.numberNew; });
        uint tn = 1;
        for (auto &t : res) {
            t.numberNew = tn;
            ++tn;
        }
        return res;
    }

    void saveTracksSettings() {
        for (auto &t : audioTracks->editors) {
            saveTrackSettings(t, "Audio");
        }
        for (auto &t : subtitleTracks->editors) {
            saveTrackSettings(t, "Subtitle");
        }
    }

    void saveTrackSettings(TrackEditor const &editor, QString const &settingsGroup) {
        trackSettings.beginGroup(settingsGroup + "_" + QString::number(editor.id));
        trackSettings.setValue("copied", editor.copiedTrackNumber->text());
        trackSettings.setValue("language", editor.language->currentText());
        trackSettings.setValue("title", editor.title->currentText());
        trackSettings.endGroup();
    }
};

ConvertDialog::ConvertDialog(Media media, Qt::WindowFlags f)
    : Dialog(nullptr, f)
    , d(GREU_MKDET(std::move(media))) {
    setObjectName("convertDialog");
    d->setupUi(this);
}

ConvertDialog::~ConvertDialog() {
    ui::Settings::instance().save(this);
}

} // namespace greu::geranos
