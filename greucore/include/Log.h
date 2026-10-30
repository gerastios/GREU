#pragma once

#include <QString>
#include "DetailPtr.h"
#include <boost/signals2.hpp>
#include <functional>

namespace greu::core {

class Exception;

class GREU_PUBLIC(CORE) Log {
public:
    enum Severity {
        Information,
        Warning,
        Error,
    };

    struct Entry {
        Severity severity;
        QString text;
    };

    Log();

    using ReaderFunction = std::function<void(Entry const &)>;
    void forEach(ReaderFunction function);

    void setActive(bool);
    bool isActive() const;

    void add(Log &);
    void add(Entry &&);
    void add(Exception const &exception);
    void addInfo(QString const &text);
    void addWarn(QString const &text);
    void addErr(QString const &text);
    void clear();

    using EntrySig = boost::signals2::signal<void(Entry const &)>;
    EntrySig &entryAdded() const;

private:
    struct Detail;
    DetailPtr<Detail> d;
};

} // namespace greu::core
