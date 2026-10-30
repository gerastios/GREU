#include "LogViewer.h"
#include <QListWidget>
#include "greucore/include/GuardedPtr.h"
#include "greucore/include/Log.h"
#include "greucore/include/signalUtils.h"

using namespace std;

namespace greu::ui {

using namespace core;

struct LogViewer::Detail {
    Log *log;
    GuardedPtr<QListWidget> list;
    vector<QListWidgetItem> items;
    ScopedConnection conn;

    Detail(Log *log)
        : log(log) {
        conn = log->entryAdded().connect(
            [this](core::Log::Entry const &entry) { addListItem(entry); });
    }

    QWidget *setupUi() {
        list = new QListWidget;
        log->forEach([this](auto entry) { addListItem(entry); });
        return list.get();
    }

    void clear() {
        for (int i = items.size() - 1; i >= 0; i--)
            list->takeItem(i);
        items.clear();
    }

    void addListItem(Log::Entry const &entry) {
        items.push_back(QListWidgetItem{icon(entry.severity), entry.text, list.get()});
        list->scrollToBottom();
    }

    QIcon icon(Log::Severity severity) {
        switch (severity) {
        case core::Log::Severity::Information:
            return list->style()->standardIcon(QStyle::SP_MessageBoxInformation);
        case core::Log::Severity::Warning:
            return list->style()->standardIcon(QStyle::SP_MessageBoxWarning);
        case core::Log::Severity::Error:
            return list->style()->standardIcon(QStyle::SP_MessageBoxCritical);
        }
        return QIcon();
    }

    ~Detail() { clear(); }
};

LogViewer::LogViewer(Log *log)
    : d(GREU_MKDET(log)) {}

QWidget *LogViewer::setupUi() {
    return d->setupUi();
}

void LogViewer::clear() {
    d->clear();
}

} // namespace greu::ui
