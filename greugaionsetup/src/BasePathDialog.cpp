#include "BasePathDialog.h"
#include <QMessageBox>
#include <QPushButton>
#include <QToolBar>
#include <QTreeView>
#include <QVBoxLayout>
#include "greucore/include/GuardedPtr.h"
#include "greuui/include/Dialog.h"
#include "greuui/include/FileSystemModel.h"
#include "greuui/include/Icons.h"
#include "greuui/include/Settings.h"
#include "greuui/include/connect.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::gaion::setup {

using namespace core;
using namespace ui;

struct BasePathDialog::Detail {
    struct MyDialog : public Dialog {
        GuardedPtr<QVBoxLayout> layout;
        QString path;
        GuardedPtr<QTreeView> tree;
        GuardedPtr<FileSystemModel> model;
        GuardedPtr<QToolBar> toolBar;
        GuardedPtr<QPushButton> ok, cancel;

        MyDialog(QString const &path)
            : Dialog("basepathdialog")
            , path(path) {}

        void setupUi() {
            layout = new QVBoxLayout;
            tree = new QTreeView();
            tree->setObjectName("basepathdialogtree");
            model = new FileSystemModel;
            model->setFilter(QDir::AllDirs | QDir::NoDotAndDotDot);
            model->setRootPath(QDir::rootPath());
            tree->setModel(model.get());
            tree->setSelectionMode(QTreeView::SingleSelection);
            layout->addWidget(tree.get());
            toolBar = new QToolBar;
            addNewWidget(ok, GREU_ICON(ok), "", toolBar.get());
            ok->setDefault(true);
            ui::connect(ok.get(), [this] {
                auto sel = tree->selectionModel()->selectedRows();
                if (sel.size() != 1) {
                    QMessageBox::warning(this,
                                         QObject::tr("Info"),
                                         QObject::tr("Please select one folder!"));
                    return;
                }
                path = model->filePath(sel.at(0));
                accept();
            });
            addNewWidget(cancel, GREU_ICON(cancel), "", toolBar.get());
            ui::connect(cancel.get(), [this] { reject(); });
            layout->addWidget(toolBar.get());
            setLayout(layout.get());
            ui::Settings::instance().restore(this);
            tree->expand(model->index(path));
            tree->setCurrentIndex(model->index(path));
            connectDirectoryLoaded(model.get(), [this](auto dir) {
                if (dir == path)
                    tree->scrollTo(model->index(path), QTreeView::PositionAtTop);
            });
        }

        ~MyDialog() { ui::Settings::instance().save(this); }
    };

    MyDialog dialog;
    Detail(QString const &current)
        : dialog(current) {}
};

BasePathDialog::BasePathDialog(QString const &current)
    : d(GREU_MKDET(current)) {}

bool BasePathDialog::execute() {
    d->dialog.setupUi();
    return d->dialog.exec();
}

QString BasePathDialog::newPath() const {
    return d->dialog.path;
}

} // namespace greu::gaion::setup
