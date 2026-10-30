#include "MainWindow.h"
#include <QGroupBox>
#include <QHBoxLayout>
#include "Application.h"
#include "Dispatcher.h"
#include "FileListTable.h"
#include "WorkFileTable.h"
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/utils.h" // IWYU pragma: keep
#include "greuui/include/Splitter.h"
#include "greuui/include/utils.h"

using namespace std;

namespace greu::geranos {

using namespace core;
using namespace ui;

struct MainWindow::Detail {
    GuardedPtr<Splitter> fileSplitter;
    GuardedPtr<QGroupBox> listBox;
    GuardedPtr<QHBoxLayout> listLayout;
    unique_ptr<FileListTable> fileList;
    GuardedPtr<WorkFileTable> fileTable;

    void setupUi() {
        fileSplitter = new Splitter(Qt::Vertical, "fileSplitter");
        fileList = make_unique<FileListTable>(GREU_D->path());
        listBox = new QGroupBox(QObject::tr("File system: ").append(GREU_D->path()));
        listLayout = new QHBoxLayout;
        listBox->setLayout(listLayout.get());
        listLayout->addWidget(fileList->setupUi());
        fileSplitter->addWidget(listBox.get());
        addNewWidget(fileTable, fileSplitter.get());
    }
};

MainWindow::MainWindow()
    : ui::MainWindow()
    , d(GREU_MKDET()) {}

FileListTable *MainWindow::fileList() const {
    return d->fileList.get();
}

WorkFileTable *MainWindow::fileTable() const {
    return d->fileTable.get();
}

void MainWindow::setupUiInt() {
    setWindowIcon(QIcon(":/geranos.jpg"));
    d->setupUi();
    setCentralWidget(d->fileSplitter.get());
    setWindowTitle(Application::instance()->winTitle());
}

} // namespace greu::geranos
