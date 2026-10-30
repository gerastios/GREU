#include "Log.h"

#include "Exception.h"

using namespace std;

namespace greu::core {

struct Log::Detail {
    vector<Entry> entries;
    bool active = true;
    EntrySig entryAdded;
};

Log::Log()
    : d(GREU_MKDET()) {}

void Log::forEach(ReaderFunction function) {
    for (auto const &e : d->entries)
        function(e);
}

void Log::setActive(bool v) {
    d->active = v;
}

bool Log::isActive() const {
    return d->active;
}

void Log::add(Log &other) {
    if (!d->active)
        return;
    for (auto e = other.d->entries.begin(); e != other.d->entries.end(); e++)
        add(std::move(*e));
}

void Log::add(Entry &&entry) {
    if (!d->active)
        return;
    d->entryAdded(entry);
    d->entries.push_back(std::move(entry));
}

void Log::add(Exception const &exception) {
    add({Severity::Error, exception.text()});
}

void Log::addInfo(QString const &text) {
    add({Severity::Information, text});
}

void Log::addWarn(QString const &text) {
    add({Severity::Warning, text});
}

void Log::addErr(QString const &text) {
    add({Severity::Error, text});
}

void Log::clear() {
    d->entries.clear();
}

Log::EntrySig &Log::entryAdded() const {
    return d->entryAdded;
}

} // namespace greu::core
