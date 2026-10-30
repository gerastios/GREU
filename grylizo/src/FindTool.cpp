#include "FindTool.h"
#include <QCheckBox>
#include <QFileInfo>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>
#include "FileTableModel.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/vectorUtils.h"
#include "greutag/include/File.h"
#include "greutag/include/Tag.h"
#include "greuui/include/Action.h"
#include "greuui/include/Icons.h"
#include "greuui/include/LineEdit.h"
#include "greuui/include/Settings.h"
#include "greuui/include/Splitter.h"
#include "greuui/include/TableView.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

namespace {
static char const *findString("FindString");
static char const *findCaseSensitive("FindCaseSensitive");
} // namespace

struct FindTool::Detail {
    struct Model : public QAbstractTableModel {
        vector<File *> foundFiles;

        Model(QWidget *parent)
            : QAbstractTableModel(parent) {}

        virtual int rowCount(QModelIndex const &) const override { return foundFiles.size(); }

        virtual int columnCount(QModelIndex const &) const override { return 3; }
        virtual QVariant data(QModelIndex const &index, int role) const override {
            if (foundFiles.size() == 0)
                return QVariant();
            if (!index.isValid())
                return QVariant();
            if ((size_t) index.row() > foundFiles.size())
                return QVariant();
            if (role != Qt::DisplayRole)
                return QVariant();
            auto f = foundFiles.at(index.row());
            switch (index.column()) {
            case 0:
                return f->info().fileName();
            case 1:
                return f->tag()->songTitle();
            case 2:
                return f->tag()->songArtist();
            default:
                return QVariant{};
            }
        }

        virtual QVariant headerData(int section,
                                    Qt::Orientation orientation,
                                    int role) const override {
            if (role != Qt::DisplayRole || orientation != Qt::Horizontal)
                return QVariant();
            switch (section) {
            case 0:
                return QObject::tr("File name");
            case 1:
                return QObject::tr("Title");
            case 2:
                return QObject::tr("Artist");
            default:
                return QVariant{};
            }
        }
    };

    FileHolder *files;

    GuardedPtr<LineEdit> edit;
    GuardedPtr<QCheckBox> caseSensitive;
    GuardedPtr<Model> model;
    GuardedPtr<QVBoxLayout> splitterLayout, tableLayout;
    GuardedPtr<QHBoxLayout> buttonLayout;
    GuardedPtr<Splitter> splitter;
    GuardedPtr<QWidget> widget, tableWidget;
    GuardedPtr<QGridLayout> layout;
    GuardedPtr<QLabel> label;
    GuardedPtr<QPushButton> find, cancel;
    GuardedPtr<TableView> table;
    GuardedPtr<Action> actGoTo;

    File *target = nullptr;
    FindTool *that;

    Detail(FileHolder *files)
        : files(files) {}

    void setupUi() {
        splitterLayout = new QVBoxLayout;
        that->setLayout(splitterLayout.get());
        splitter = new Splitter(Qt::Vertical, "findSplitter");
        splitterLayout->addWidget(splitter.get());
        layout = new QGridLayout;
        addNewWidget(widget, splitter.get());
        widget->setLayout(layout.get());
        addNewWidget(label, QObject::tr("&Search string"), layout.get(), 0, 0);
        addNewWidget(edit, layout.get(), 0, 1);
        edit->setText(ui::Settings::instance().value(findString, "").toString());
        label->setBuddy(edit.get());
        addNewWidget(caseSensitive, QObject::tr("&Case sensitive"), layout.get(), 1, 0, 1, 2);
        caseSensitive->setChecked(ui::Settings::instance().value(findCaseSensitive, false).toBool());
        find = new QPushButton(GREU_ICON(find), "");
        cancel = new QPushButton(GREU_ICON(cancel), "");
        ui::connect(cancel.get(), [this] { that->reject(); });
        ui::connect(find.get(), [this] {
            model->layoutAboutToBeChanged();
            model->foundFiles.clear();
            auto toFind = edit->text();
            files->forEach([this, &toFind](tag::File *f) {
                auto tag = f->tag();
                if (searchText(tag->fileName(), toFind) || searchText(tag->songTitle(), toFind)
                    || searchText(tag->songArtist(), toFind))
                    model->foundFiles.push_back(f);
            });
            if (model->foundFiles.size() > 0) {
                model->layoutChanged();
            } else {
                QMessageBox::information(that, QObject::tr("Find"), QObject::tr("Nothing found"));
            }
        });
        buttonLayout = new QHBoxLayout;
        buttonLayout->addWidget(find.get());
        buttonLayout->addWidget(cancel.get());
        layout->addLayout(buttonLayout.get(), 2, 0, 1, 2);

        tableWidget = new QWidget;
        tableLayout = new QVBoxLayout;
        tableWidget->setLayout(tableLayout.get());
        table = new TableView("FindResult");
        tableWidget->setMinimumHeight(50);
        tableLayout->addWidget(table.get());
        model = new Model(table.get());
        table->setModel(model.get());
        splitter->addWidget(tableWidget.get());
        connectDoubleClicked(table.get(),
                             [this](QModelIndex const &index) { selectFile(index.row()); });
        actGoTo = new Action("&Go to file");
        table->addContextAction(
            {actGoTo.get(), [this](int row, int, QAbstractTableModel *) { selectFile(row); }});
        ui::Settings::instance().restore(that);
    }

    tag::File *at(size_t index) {
        try {
            return model->foundFiles.at(index);
        } catch (...) {
        }
        return nullptr;
    }

    virtual ~Detail() {
        ui::Settings::instance().save(that);
        ui::Settings::instance().setValue(findString, edit->text());
        ui::Settings::instance().setValue(findCaseSensitive, caseSensitive->isChecked());
    }

    bool searchText(QString const &text, QString const &toFind) {
        return text.indexOf(toFind,
                            0,
                            caseSensitive->isChecked() ? Qt::CaseSensitive : Qt::CaseInsensitive)
               >= 0;
    }

    void selectFile(int row) {
        try {
            auto fc = model->foundFiles.at(row);
            target = fc;
            that->accept();
        } catch (...) {
        }
    }
};

FindTool::FindTool(FileHolder *files)
    : d(GREU_MKDET(files)) {
    d->that = this;
    setObjectName("FindDialog");
    d->setupUi();
}

File *FindTool::targetFile() const {
    return d->target;
}

size_t FindTool::size() const {
    return d->model->foundFiles.size();
}

File *FindTool::at(size_t index) {
    return d->at(index);
}

optional<size_t> FindTool::indexOf(File *file) {
    return core::indexOf(d->model->foundFiles, file);
}

void FindTool::cforEach(ReaderFunction function) const {
    for (auto file : d->model->foundFiles) {
        function(file);
    }
}

void FindTool::forEach(WriterFunction function) {
    for (auto file : d->model->foundFiles) {
        function(file);
    }
}

} // namespace greu::grylizo
