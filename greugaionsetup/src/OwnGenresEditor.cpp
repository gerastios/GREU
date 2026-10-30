#include "OwnGenresEditor.h"
#include <QApplication>
#include <QClipboard>
#include <QDialogButtonBox>
#include <QGridLayout>
#include <QLabel>
#include <QMimeData>
#include <QPushButton>
#include "greucore/include/GuardedPtr.h"
#include "greumovies/include/GenresDefinitionTable.h"
#include "greuui/include/Action.h"
#include "greuui/include/ComboBox.h"
#include "greuui/include/Icons.h"
#include "greuui/include/TableView.h"
#include "greuui/include/connect.h"

using namespace std;

namespace greu::gaion::setup {

using namespace core;
using namespace ui;
using namespace movies;

struct OwnGenresEditor::Detail {
    struct GenresModel : public QAbstractTableModel {
        OwnMovie const &ownMovie;

#define GENRE 0

        GenresModel(OwnMovie const &ow)
            : ownMovie(ow) {}

        virtual int rowCount(QModelIndex const &) const override { return ownMovie.genres.size(); }

        virtual int columnCount(QModelIndex const &) const override { return GENRE + 1; }

        virtual QVariant data(QModelIndex const &index, int role) const override {
            if (role != Qt::DisplayRole || index.row() >= (int) ownMovie.genres.size())
                return {};
            auto &g = (ownMovie.genres[index.row()]);
            switch (index.column()) {
            case GENRE:
                return g;
            }
            return {};
        }

        void update() {
            layoutAboutToBeChanged();
            layoutChanged();
        }
    };

    OwnMovie ownMovie;
    GuardedPtr<QGridLayout> layout;
    GuardedPtr<TableView> genresTable;
    unique_ptr<GenresModel> model;
    GuardedPtr<QLabel> genreLabel;
    GuardedPtr<ComboBox> genre;
    GuardedPtr<QDialogButtonBox> buttons;
    GuardedPtr<QPushButton> ok, cancel, addGenre;
    GuardedPtr<Action> actDelete;

    Detail(OwnMovie const &om)
        : ownMovie(om) {
        actDelete = new Action(QObject::tr("Delete line"));
    }

    void setupUi(OwnGenresEditor *that) {
        layout = new QGridLayout;
        int row = 0;
        genresTable = new TableView("owngenrestable");
        genresTable->addContextAction(actDelete.get(), [this](int row, auto, auto) {
            ownMovie.genres.erase(ownMovie.genres.begin() + row);
        });
        model = make_unique<GenresModel>(ownMovie);
        genresTable->setModel(model.get());
        genreLabel = new QLabel(QObject::tr("Genre"));
        genre = new ComboBox;
        genre->setEditable(false);
        auto genres = GenresDefinitionTable::instance()->allMovieGenres();
        for (auto &g : genres)
            genre->addItem(g);
        buttons = new QDialogButtonBox;
        ok = new QPushButton(GREU_ICON(ok), "");
        cancel = new QPushButton(GREU_ICON(cancel), "");
        addGenre = new QPushButton(QObject::tr("Add genre"));
        buttons->addButton(ok.get(), QDialogButtonBox::AcceptRole);
        buttons->addButton(cancel.get(), QDialogButtonBox::RejectRole);
        buttons->addButton(addGenre.get(), QDialogButtonBox::ActionRole);
        layout->addWidget(genresTable.get(), row, 0, 10, 2);
        row += 10;
        layout->addWidget(genreLabel.get(), row, 0);
        layout->addWidget(genre.get(), row++, 1);
        layout->addWidget(buttons.get(), row++, 0, 1, 2);
        ui::connect(ok.get(), [that] { that->accept(); });
        ui::connect(cancel.get(), [that] { that->reject(); });
        ui::connect(addGenre.get(), [this] {
            if (genre->currentText().isEmpty())
                return;
            ownMovie.genres.push_back(genre->currentText());
            model->update();
            genre->setCurrentText("");
        });
    }
};

OwnGenresEditor::OwnGenresEditor(OwnMovie const &om)
    : d(GREU_MKDET(om)) {
    d->setupUi(this);
    setLayout(d->layout.get());
}

OwnMovie OwnGenresEditor::ownMovie() const {
    return d->ownMovie;
}

} // namespace greu::gaion::setup
