#include "DuplicateTool.h"
#include <QColor>
#include <QContextMenuEvent>
#include <QHeaderView>
#include <QPushButton>
#include <QVBoxLayout>
#include "Application.h"
#include "Dispatcher.h"
#include "FileTableModel.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/stringUtils.h"
#include "greucore/include/utils.h"
#include "greutag/include/AudioCollection.h"
#include "greutag/include/File.h"
#include "greutag/include/FileHolder.h"
#include "greutag/include/Tag.h"
#include "greuui/include/Action.h"
#include "greuui/include/Dialog.h"
#include "greuui/include/Icons.h"
#include "greuui/include/Settings.h"
#include "greuui/include/TableView.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"
#include <boost/pointer_cast.hpp>
#include <unordered_map>
#include <vector>

using namespace std;

namespace greu::grylizo {

using namespace core;
using namespace ui;
using namespace tag;

struct DuplicateTool::Detail {
    struct DuplicateEntry {
        File *file;
        bool isFirst;
    };
    using Vector = vector<DuplicateEntry>;

    struct Holder : public virtual FileHolder {
        Vector objects;

        virtual size_t size() const override { return objects.size(); }

        virtual File *at(size_t index) override { return objects.at(index).file; }

        virtual std::optional<size_t> indexOf(File *file) override {
            auto res = find_if(objects.begin(), objects.end(), [file](auto &f) {
                return f.file == file;
            });
            if (res == objects.end())
                return nullopt;
            return res - objects.begin();
        }

        virtual void cforEach(ReaderFunction function) const override {
            for (auto &e : objects) {
                function(e.file);
            }
        }

        virtual void forEach(WriterFunction function) override {
            for (auto const &e : objects) {
                function(e.file);
            }
        }
    };

    struct Model : public FileTableModel {
        GuardedPtr<Action> actDelFile, actGotoFile;

        Model(TableView *table, Holder *container, DuplicateTool *that)
            : FileTableModel(container,
                             table,
                             {AttributeTypes::fileName(),
                              AttributeTypes::songTitle(),
                              AttributeTypes::songArtist()}) {
            actDelFile = new Action(QObject::tr("&Delete file"));
            actGotoFile = new Action(QObject::tr("&Go to file"));
            connectDoubleClicked(table, [that](QModelIndex const &index) {
                that->d->gotoFile(index.row());
            });
            table->addContextAction(actGotoFile.get(), [that](int row, int, QAbstractTableModel *) {
                that->d->gotoFile(row);
            });
            table->addContextAction(actDelFile.get(), [that](int row, int, QAbstractTableModel *) {
                if (row < 0 || row >= int(that->d->duplicates.size()))
                    return;
                that->d->deleteFile(row);
            });
            table->setSortingEnabled(false);
            table->verticalHeader()->setSortIndicatorShown(false);
            table->setSelectionMode(QAbstractItemView::NoSelection);
        }

        static QColor &color1stLine() {
            static QColor c{Qt::lightGray};
            return c;
        }

        static QColor &colorName() {
            static QColor c{Qt::green};
            return c;
        }

        static QColor &colorAtts() {
            static QColor c{Qt::blue};
            return c;
        }

        virtual optional<QColor> color(QModelIndex const &index) override {
            if (safeCast<Holder>(fileHolder())->objects.at(index.row()).isFirst)
                return color1stLine();
            if (index.column() == 0)
                return colorName();
            return colorAtts();
        }
    };

    struct Viewer : public Dialog {
        GuardedPtr<TableView> table;
        GuardedPtr<Model> model;
        GuardedPtr<QVBoxLayout> layout;
        GuardedPtr<QPushButton> ok;

        Viewer(Holder *duplicates, DuplicateTool *that) {
            setObjectName("duplDialog");
            layout = new QVBoxLayout;
            setLayout(layout.get());
            addNewWidget(table, "duplView", layout.get());
            model = new Model(table.get(), duplicates, that);
            table->setModel(model.get());
            addNewWidget(ok, GREU_ICON(ok), "", layout.get());
            ui::connect(ok.get(), [this] { reject(); });
            ui::Settings::instance().restore(this);
            table->noSorting();
        }

        virtual void closeEvent(QCloseEvent *) override { ui::Settings::instance().save(this); }
    };

    DuplicateTool *that;
    FileHolder *const files;
    Holder duplicates;
    File *target = nullptr;
    Viewer *viewer;

    Detail(FileHolder *files)
        : files(files) {}

    bool searchDuplicates() {
        duplicates.objects.clear();
        std::unordered_map<core::StringPair, std::vector<tag::File *>> dups;
        files->forEach([&dups](tag::File *file) {
            auto tag = file->tag();
            auto key = StringPair{tag->songTitle(), tag->songArtist()};
            bool found = false;
            for (auto d = dups.begin(); d != dups.end() && !found; d++) {
                if (fuzzyCompare(d->first.first, key.first)
                    && fuzzyCompare(d->first.second, key.second)) {
                    d->second.push_back(file);
                    found = true;
                }
            }
            if (!found) {
                dups.insert({key, vector<tag::File *>()});
                dups[key].push_back(file);
            }
        });
        std::vector<StringPair> noDups;
        for (auto &d : dups) {
            if (d.second.size() < 2)
                noDups.push_back(d.first);
        }
        for (auto &d : noDups) {
            dups.erase(d);
        }
        for (auto &d : dups) {
            bool first = true;
            for (auto &f : d.second) {
                duplicates.objects.push_back(DuplicateEntry{.file = f, .isFirst = first});
                first = false;
            }
        }
        return dups.size() > 0;
    }

    void viewDuplicates() {
        viewer = new Viewer(&duplicates, that);
        viewer->exec();
    }

    void gotoFile(int row) {
        if (row < 0 || (size_t) row >= duplicates.size())
            return;
        target = duplicates.at(row);
        viewer->accept();
    }

    void deleteFile(int row) {
        if (row < 0 || (size_t) row >= duplicates.size())
            return;
        if (GREU_D->audioCollection()->deleteFile(duplicates.at(row)))
            viewer->accept();
    }
};

DuplicateTool::DuplicateTool(FileHolder *files)
    : d(GREU_MKDET(files)) {
    d->that = this;
}

bool DuplicateTool::execute() {
    if (d->searchDuplicates()) {
        d->viewDuplicates();
        return true;
    }
    return false;
}

File *DuplicateTool::targetFile() const {
    return d->target;
}

} // namespace greu::grylizo
