#include "Actions.h"
#include <QAudioOutput>
#include <QFileInfo>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QMouseEvent>
#include <QToolBar>
#include "AbstractAttributeEditor.h"
#include "Application.h"
#include "AttributeEditor.h"
#include "CompoundAttributeEditor.h"
#include "Dispatcher.h"
#include "DuplicateTool.h"
#include "FileEditor.h"
#include "FileTable.h"
#include "FindTool.h"
#include "MainEditor.h"
#include "MainWindow.h"
#include "NormalizeTool.h"
#include "SettingsDialog.h"
#include "greucore/include/GuardedPtr.h"
#include "greutag/include/AttributeType.h"
#include "greutag/include/AudioCollection.h"
#include "greutag/include/File.h"
#include "greutag/include/FileContainer.h"
#include "greutag/include/FileHolder.h"
#include "greutag/include/Tag.h"
#include "greuui/include/Action.h"
#include "greuui/include/Icons.h"
#include "greuui/include/LabelledSlider.h"
#include "greuui/include/MediaPlayer.h"
#include "greuui/include/Slider.h"
#include "greuui/include/TableView.h"
#include "greuui/include/connect.h"
#include <functional>
#include <random>

using namespace std;

namespace greu::grylizo {

using namespace greu::core;
using namespace greu::ui;
using namespace greu::tag;

struct Actions::Detail {
    using TriggerFunc = function<void()>;

    GuardedPtr<QMenuBar> menu;
    GuardedPtr<QMenu> menuFile;
    GuardedPtr<Action> actSave, actSaveAll, actDeleteFile, actExit;
    GuardedPtr<QMenu> menuEdit;
    GuardedPtr<Action> actSelectAll, actDeselectAll, actPreferences;
    GuardedPtr<QMenu> menuGoto;
    GuardedPtr<Action> actNextFile, actPrevFile;
    GuardedPtr<QMenu> menuUtilities;
    GuardedPtr<Action> actTrackNrByFileName, actRenumberTracks, actShuffleTracks, actFind,
        actFindDuplicates, actNormalize, actCopyAttributes, actPasteAttributes;
    GuardedPtr<QMenu> menuPlayer;
    GuardedPtr<Action> actPlay, actPause, actStop, actLouder, actSofter;
    GuardedPtr<QMenu> menuHelp;
    GuardedPtr<Action> actAbout;
    GuardedPtr<QToolBar> tools;
    GuardedPtr<LabelledSlider> playerSlider;

    ScopedConnection containerConn, fileConn, sliderConn;

    unordered_map<uint64_t, Attribute const *> savedAttributes;

    void makeActions() {
        makeActionSave();
        makeActionSaveAll();
        makeActionExit();
        makeActionSelectAll();
        makeActionDeselectAll();
        makeActionPreferences();
        makeActionFind();
        makeActionNextFile();
        makeActionPrevFile();
        makeActionTrackNrByFileName();
        makeActionRenumberTracks();
        makeActionShuffleTracks();
        makeActionFindDuplicates();
        makeActionNormalize();
        makeActionCopyAttribtes();
        makeActionPasteAttribtes();
        makeActionPlay();
        makeActionPause();
        makeActionLouder();
        makeActionSofter();
        makeActionStop();
        makeActionAbout();
        containerConn = GREU_M->fileTable()->containerSet().connect(
            [this](FileContainer *container) {
                actTrackNrByFileName->setEnabled(container);
                actRenumberTracks->setEnabled(container);
                actShuffleTracks->setEnabled(container);
                actFindDuplicates->setEnabled(container);
                actFind->setEnabled(container);
                actNormalize->setEnabled(container);
                actSelectAll->setEnabled(container);
                actDeselectAll->setEnabled(container);
                enableFileSteps();
            });
        fileConn = GREU_M->fileEditor()->fileSet().connect([this](auto) {
            enableFileSteps();
            enablePlaying();
            enableAttributesCopy();
        });
    }

    GuardedPtr<Action> makeAction(QString const &text,
                                  QKeySequence const hotkey,
                                  Icons::Name iconName,
                                  TriggerFunc triggerFunction,
                                  bool enabled = true) {
        GuardedPtr<Action> res = new Action{text, hotkey, Icons::icon(iconName)};
        GREU_M->addAction(res.get());
        ui::connect(res.get(), triggerFunction);
        res->setEnabled(enabled);
        return res;
    }

    void makeActionSave() {
        actSave = makeAction(
            QObject::tr("&Save modified"),
            QKeySequence(Qt::CTRL | Qt::Key_S),
            Icons::Name::save,
            [] { GREU_D->saveModified(false); },
            true);
    }

    void makeActionSaveAll() {
        actSaveAll = makeAction(
            QObject::tr("&Save all"),
            QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_S),
            Icons::Name::save_all,
            [] { GREU_D->saveAll(true); },
            true);
    }

    void makeActionExit() {
        actExit = makeAction(QObject::tr("E&xit"),
                             QKeySequence(Qt::CTRL | Qt::Key_X),
                             Icons::Name::close,
                             [] { GREU_M->close(); });
    }

    void makeActionSelectAll() {
        actSelectAll = makeAction(
            QObject::tr("Select &all"),
            QKeySequence(Qt::CTRL | Qt::Key_A),
            Icons::Name::select_all,
            [] { GREU_M->fileTable()->table()->selectAll(); },
            false);
    }

    void makeActionDeselectAll() {
        actDeselectAll = makeAction(
            QObject::tr("Deselect a&ll"),
            QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_A),
            Icons::Name::select_none,
            [] { GREU_M->fileTable()->table()->clearSelection(); },
            false);
    }

    void makeActionFind() {
        actFind = makeAction(
            QObject::tr("Find songs"),
            QKeySequence(Qt::CTRL | Qt::Key_F),
            Icons::Name::find,
            [] {
                auto container = GREU_M->fileTable()->fileContainer();
                if (!container)
                    return;
                GuardedPtr<FindTool> tool = new FindTool(container);
                if (tool->exec() == QDialog::Accepted) {
                    auto f = tool->targetFile();
                    if (f)
                        GREU_M->gotoFile(f);
                }
            },
            false);
    }

    void makeActionNextFile() {
        actNextFile = makeAction(
            QObject::tr("&Next file"),
            QKeySequence(Qt::ALT | Qt::Key_Down),
            Icons::Name::down,
            [] { GREU_M->relativeFile(1); },
            false);
    }

    void makeActionPrevFile() {
        actPrevFile = makeAction(
            QObject::tr("&Previous file"),
            QKeySequence(Qt::ALT | Qt::Key_Up),
            Icons::Name::up,
            [] { GREU_M->relativeFile(-1); },
            false);
    }

    void makeActionTrackNrByFileName() {
        actTrackNrByFileName = makeAction(
            QObject::tr("Set &track number by file name"),
            QKeySequence(Qt::CTRL | Qt::Key_N),
            Icons::Name::assign,
            [this] {
                GREU_M->endEditing();
                auto files = GREU_M->fileTable()->selectedFiles();
                auto cf = GREU_M->fileEditor()->file();
                for (auto f = files.begin(); f != files.end(); f++) {
                    auto tn = (*f)->tag()->attributeTrackNumber();
                    if (tn->value() == 0) {
                        auto fn = (*f)->tag()->fileName().simplified();
                        bool ok = false;
                        uint n;
                        for (int len = fn.length(); len > 0 && !ok; len--)
                            n = fn.left(len).toUInt(&ok);
                        if (ok) {
                            tn->setValue(n);
                            if (cf && *f == cf)
                                updateAttributeEditor(GREU_M->fileEditor()->mainEditor(),
                                                      tag::AttributeTypes::trackNumber());
                        }
                    }
                }
            },
            false);
    }

    void makeActionRenumberTracks() {
        actRenumberTracks = makeAction(
            QObject::tr("&Renumber tracks"),
            QKeySequence(Qt::CTRL | Qt::Key_R),
            Icons::Name::remove_gaps,
            [this] {
                GREU_M->endEditing();
                auto container = GREU_M->fileTable()->fileContainer();
                if (!container)
                    return;
                auto count = container->size();
                auto cf = GREU_M->fileEditor()->file();
                std::vector<tag::File *> toNumber;
                std::unordered_map<size_t, tag::File *> numbered;
                for (auto f = container->begin(); f != container->end(); ++f) {
                    auto tn = (*f)->tag()->trackNumber();
                    if (tn == 0 || numbered.contains(tn))
                        toNumber.push_back(*f);
                    else
                        numbered[tn] = *f;
                }
                size_t nr = 1;
                while (nr <= count) {
                    if (!numbered.contains(nr)) {
                        if (toNumber.size() > 0) {
                            toNumber[0]->tag()->setTrackNumber(nr);
                            if (cf && toNumber[0] == cf)
                                updateAttributeEditor(GREU_M->fileEditor()->mainEditor(),
                                                      tag::AttributeTypes::trackNumber());
                            toNumber.erase(toNumber.begin());
                        } else {
                            size_t max = 0;
                            auto del = numbered.begin();
                            for (auto i = numbered.begin(); i != numbered.end(); i++) {
                                if (i->first > max) {
                                    max = i->first;
                                    del = i;
                                }
                            }
                            numbered[max]->tag()->setTrackNumber(nr);
                            if (numbered[max] == cf)
                                updateAttributeEditor(GREU_M->fileEditor()->mainEditor(),
                                                      tag::AttributeTypes::trackNumber());
                            numbered.erase(del);
                        }
                    }
                    nr++;
                }
            },
            false);
    }

    void makeActionShuffleTracks() {
        actShuffleTracks = makeAction(
            QObject::tr("S&huffle tracks"),
            QKeySequence(Qt::CTRL | Qt::Key_H),
            Icons::Name::shuffle,
            [this] {
                GREU_M->endEditing();
                auto container = GREU_M->fileTable()->fileContainer();
                if (!container)
                    return;
                auto cf = GREU_M->fileEditor()->file();
                std::vector<tag::File *> unnumbered;
                for (auto f = container->begin(); f != container->end(); f++)
                    unnumbered.push_back(*f);
                std::mt19937 rng(std::chrono::steady_clock::now().time_since_epoch().count());
                std::shuffle(unnumbered.begin(), unnumbered.end(), rng);
                uint i = 1;
                for (auto f : unnumbered) {
                    f->tag()->setTrackNumber(i++);
                    if (f == cf)
                        updateAttributeEditor(GREU_M->fileEditor()->mainEditor(),
                                              tag::AttributeTypes::trackNumber());
                }
            },
            false);
    }

    void makeActionFindDuplicates() {
        actFindDuplicates = makeAction(
            QObject::tr("Find &duplicates"),
            QKeySequence(Qt::CTRL | Qt::Key_D),
            Icons::Name::twins,
            [] {
                GREU_M->endEditing();
                auto container = GREU_M->fileTable()->fileContainer();
                if (!container)
                    return;
                DuplicateTool tool{container};
                if (tool.execute()) {
                    auto f = tool.targetFile();
                    if (f)
                        GREU_M->gotoFile(f);
                } else {
                    QMessageBox::information(GREU_M,
                                             QObject::tr("Find duplicaates"),
                                             QObject::tr("No duplicates found"));
                }
            },
            false);
    }

    void makeActionNormalize() {
        actNormalize = makeAction(
            QObject::tr("&Normalize files"),
            QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_N),
            Icons::Name::normalize,
            [] {
                GREU_M->endEditing();
                auto container = GREU_M->fileTable()->fileContainer();
                if (!container)
                    return;
                if (GREU_D->audioCollection()->isModified()) {
                    QMessageBox::information(GREU_M,
                                             QObject::tr("Normalize"),
                                             QObject::tr("Please save data first!"));
                    return;
                }
                auto files = GREU_M->fileTable()->selectedFiles();
                if (files.size() == 0) {
                    QMessageBox::information(GREU_M,
                                             QObject::tr("Normalize"),
                                             QObject::tr("Please select files to process!"));
                    return;
                }
                GuardedPtr<NormalizeTool> tool = new NormalizeTool(files);
                tool->exec();
                for (auto f = container->begin(); f != container->end(); f++) {
                    (*f)->updateInfo();
                }
            },
            false);
    }

    void makeActionCopyAttribtes() {
        actCopyAttributes = makeAction(
            QObject::tr("&Copy attributes"),
            QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_C),
            Icons::Name::copy,
            [this] {
                auto file = GREU_M->fileEditor()->file();
                if (!file)
                    return;
                GREU_M->endEditing();
                savedAttributes.clear();
                for (auto const &a : file->tag()->attributes())
                    savedAttributes.insert({a.first, a.second.attribute.get()});
            },
            false);
    }

    void makeActionPasteAttribtes() {
        actPasteAttributes = makeAction(
            QObject::tr("&Paste attributes"),
            QKeySequence(Qt::CTRL | Qt::ALT | Qt::Key_V),
            Icons::Name::paste,
            [this] {
                auto file = GREU_M->fileEditor()->file();
                if (!file || savedAttributes.empty())
                    return;
                GREU_M->endEditing();
                for (auto a : savedAttributes) {
                    auto an = file->tag()->attribute(a.first);
                    an->copyValue(*a.second);
                }
            },
            false);
    }

    void makeActionPlay() {
        actPlay = makeAction(
            QObject::tr("&Play song"),
            QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P),
            Icons::Name::play,
            [this] {
                auto mw = GREU_M;
                mw->endEditing();
                mw->player()->stop();
                playerSlider->setEnabled(false);
                auto file = mw->fileEditor()->file();
                if (!file)
                    return;
                if (file->isModified()) {
                    QMessageBox::information(GREU_M,
                                             QObject::tr("Player"),
                                             QObject::tr("Please save file first!"));
                    return;
                }
                mw->player()->setSource(QUrl::fromLocalFile(file->info().canonicalFilePath()));
                playerSlider->label()->setText(
                    QObject::tr("%1 (%2)").arg(file->tag()->songTitle(), file->tag()->songArtist()));
                playerSlider->setEnabled(true);
                mw->player()->play();
            },
            false);
    }

    void makeActionPause() {
        actPause = makeAction(
            QObject::tr("Pa&use playing"),
            QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_U),
            Icons::Name::pause,
            [] {
                if (GREU_M->player()->playbackState() == QMediaPlayer::PlayingState)
                    GREU_M->player()->pause();
                else if (GREU_M->player()->playbackState() == QMediaPlayer::PausedState)
                    GREU_M->player()->play();
            },
            false);
    }

    void makeActionStop() {
        actStop = makeAction(
            QObject::tr("&Stop playing"),
            QKeySequence(Qt::ALT | Qt::SHIFT | Qt::Key_S),
            Icons::Name::stop,
            [this] {
                GREU_M->player()->stop();
                playerSlider->setEnabled(false);
            },
            false);
    }

    void makeActionLouder() {
        actLouder = makeAction(
            QObject::tr("&Volume up"),
            QKeySequence(Qt::ALT | Qt::SHIFT | Qt::Key_Plus),
            Icons::Name::volume_high,
            [] {
                auto a = GREU_M->player()->audioOutput();
                a->setVolume(a->volume() + 4);
            },
            false);
    }

    void makeActionSofter() {
        actSofter = makeAction(
            QObject::tr("&Volume down"),
            QKeySequence(Qt::ALT | Qt::SHIFT | Qt::Key_Minus),
            Icons::Name::volume_low,
            [] {
                auto a = GREU_M->player()->audioOutput();
                a->setVolume(a->volume() - 4);
            },
            false);
    }

    void makeActionPreferences() {
        actPreferences = makeAction(QObject::tr("&Preferences"),
                                    QKeySequence(Qt::CTRL | Qt::SHIFT | Qt::Key_P),
                                    Icons::Name::settings,
                                    [] {
                                        GREU_M->endEditing();
                                        GuardedPtr<SettingsDialog> d = new SettingsDialog;
                                        d->exec();
                                    });
    }

    void makeActionAbout() {
        actAbout = makeAction(
            QObject::tr("&About"), QKeySequence(Qt::CTRL | Qt::Key_F1), Icons::Name::about, [] {
                GREU_M->endEditing();
                QMessageBox::information(
                    GREU_M,
                    QObject::tr("About Grylizo"),
                    QObject::tr(
                        "Grylizo Audio Editor %1\n© 2020-2023 Gerd Rother\ngerd.rother@gmx.net")
                        .arg(Application::instance()->applicationVersion()),
                    QMessageBox::Button::Ok);
                ;
            });
    }

    void makeMenu() {
        menu = new QMenuBar;

        menuFile = new QMenu(QObject::tr("&File"));
        menuFile->addAction(actSave.get());
        menuFile->addAction(actSaveAll.get());
        menuFile->addSeparator();
        menuFile->addAction(actExit.get());

        menuEdit = new QMenu(QObject::tr("&Edit"));
        menuEdit->addAction(actSelectAll.get());
        menuEdit->addAction(actDeselectAll.get());
        menuEdit->addAction(actFind.get());
        menuEdit->addSeparator();
        menuEdit->addAction(actPreferences.get());

        menuGoto = new QMenu(QObject::tr("&Goto"));
        menuGoto->addAction(actNextFile.get());
        menuGoto->addAction(actPrevFile.get());

        menuUtilities = new QMenu(QObject::tr("&Utilities"));
        menuUtilities->addAction(actTrackNrByFileName.get());
        menuUtilities->addAction(actRenumberTracks.get());
        menuUtilities->addAction(actShuffleTracks.get());
        menuUtilities->addAction(actFindDuplicates.get());
        menuUtilities->addAction(actNormalize.get());
        menuEdit->addSeparator();
        menuUtilities->addAction(actCopyAttributes.get());
        menuUtilities->addAction(actPasteAttributes.get());

        menuPlayer = new QMenu(QObject::tr("&Player"));
        menuPlayer->addAction(actPlay.get());
        menuPlayer->addAction(actPause.get());
        menuPlayer->addAction(actStop.get());
        menuPlayer->addAction(actLouder.get());
        menuPlayer->addAction(actSofter.get());
        menuUtilities->addMenu(menuPlayer.get());

        menuHelp = new QMenu(QObject::tr("&Help"));
        menuHelp->addAction(actAbout.get());

        menu->addMenu(menuFile.get());
        menu->addMenu(menuEdit.get());
        menu->addMenu(menuGoto.get());
        menu->addMenu(menuUtilities.get());
        menu->addMenu(menuHelp.get());

        GREU_M->setMenuBar(menu.get());
    }

    void makeToolbar() {
        tools = new QToolBar;
        tools->setObjectName("mainToolBar");
        tools->addAction(actSave.get());
        tools->addSeparator();
        tools->addAction(actSelectAll.get());
        tools->addAction(actDeselectAll.get());
        tools->addAction(actFind.get());
        tools->addSeparator();
        tools->addAction(actTrackNrByFileName.get());
        tools->addAction(actRenumberTracks.get());
        tools->addAction(actShuffleTracks.get());
        tools->addAction(actFindDuplicates.get());
        tools->addAction(actNormalize.get());
        tools->addSeparator();
        tools->addAction(actNextFile.get());
        tools->addAction(actPrevFile.get());
        tools->addSeparator();
        tools->addAction(actCopyAttributes.get());
        tools->addAction(actPasteAttributes.get());
        tools->addSeparator();
        tools->addAction(actPlay.get());
        tools->addAction(actPause.get());
        tools->addAction(actStop.get());
        tools->addAction(actLouder.get());
        tools->addAction(actSofter.get());
        makePlayerSlider();
        tools->addWidget(playerSlider.get());
        GREU_M->addToolBar(tools.get());
    }

    void makePlayerSlider() {
        int const max = 6000;
        auto player = GREU_M->player();
        auto playerToSlider = [player](int64_t p) {
            auto d = player->duration();
            if (d == 0)
                return 0;
            return (int) ((p * max) / d);
        };
        auto sliderToPlayer = [player](int s) {
            auto d = player->duration();
            if (d == 0)
                return (int64_t) 0;
            return ((int64_t) d * s) / max;
        };
        auto playerChanged = [this, playerToSlider, player](auto) {
            if (playerSlider->slider()->isSliderDown())
                return;
            if (player->playbackState() == QMediaPlayer::PlayingState
                || player->playbackState() == QMediaPlayer::PausedState) {
                playerSlider->setEnabled(true);
                playerSlider->slider()->setSliderPosition(playerToSlider(player->position()));
            } else {
                playerSlider->setEnabled(false);
            }
        };
        playerSlider = new LabelledSlider;
        playerSlider->setMaximumWidth(200);
        playerSlider->setEnabled(false);
        playerSlider->slider()->setMinimum(0);
        playerSlider->slider()->setMaximum(max);
        ui::connectDurationChanged(player, playerChanged);
        ui::connectPositionChanged(player, playerChanged);
        ui::connectReleased(playerSlider->slider(), [this, sliderToPlayer, player] {
            if (player->playbackState() == QMediaPlayer::PlayingState
                || player->playbackState() == QMediaPlayer::PausedState) {
                player->setPosition(sliderToPlayer(playerSlider->slider()->sliderPosition()));
            }
        });
        sliderConn = playerSlider->slider()->mouseReleased().connect(
            [this, sliderToPlayer, player](auto event) {
                auto s = playerSlider->slider();
                if (s->isSliderDown())
                    return;
                if (event->button() == Qt::LeftButton) {
                    player->setPosition(sliderToPlayer(
                        s->minimum()
                        + ((s->maximum() - s->minimum()) * event->position().x()) / s->width()));
                    event->accept();
                }
            });
    }

    void updateAttributeEditor(AbstractAttributeEditor *editor, uint id) {
        auto attEd = dynamic_cast<AttributeEditor *>(editor);
        if (attEd) {
            if (attEd->type()->id())
                attEd->update();
            return;
        }
        auto compEd = dynamic_cast<CompoundAttributeEditor *>(editor);
        if (compEd)
            for (auto ed = compEd->begin(); ed != compEd->end(); ++ed) {
                updateAttributeEditor((*ed).get(), id);
            }
    }

    void enableFileSteps() {
        auto enabled = GREU_M->fileTable()->fileContainer() && GREU_M->fileEditor()->file();
        actNextFile->setEnabled(enabled);
        actPrevFile->setEnabled(enabled);
    }

    void enablePlaying() {
        auto enabled = GREU_M->fileTable()->fileContainer() && GREU_M->fileEditor()->file();
        actPlay->setEnabled(enabled);
        actPause->setEnabled(enabled);
        actStop->setEnabled(enabled);
        actLouder->setEnabled(enabled);
        actSofter->setEnabled(enabled);
    }

    void enableAttributesCopy() {
        auto enabled = GREU_M->fileTable()->fileContainer() && GREU_M->fileEditor()->file();
        actCopyAttributes->setEnabled(enabled);
        actPasteAttributes->setEnabled(enabled);
    }
};

Actions::Actions()
    : d(GREU_MKDET()) {
    d->makeActions();
    d->makeToolbar();
    d->makeMenu();
}

} // namespace greu::grylizo
