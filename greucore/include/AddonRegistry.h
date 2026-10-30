#pragma once

#include <QDir>
#include <QFileInfoList>
#include <QLibrary>
#include <QString>
#include "greuGlobal.h"
#include "vectorUtils.h"
#include <mutex>

class QFileInfo;

namespace greu::core {

template<class Interface, typename Key>
class GREU_PUBLIC(CORE) AddonRegistry {
public:
    struct Addon {
        Interface *handler;
        QString name;
        bool operator==(Addon const &other) const { return handler == other.handler; }
    };
    using Addons = std::vector<Addon>;

    void registerAddon(Interface *a, QString const &name) {
        if (!a)
            return;
        std::lock_guard<std::recursive_mutex> const lock(_mutex);
        if (greu::core::contains(_addons, {a, name}))
            return;
        _addons.push_back({a, name});
    }

    Addons const &addons() const {
        std::lock_guard<std::recursive_mutex> const lock(_mutex);
        return _addons;
    }

    Addon const *instanceFor(Key const &k) const {
        std::lock_guard<std::recursive_mutex> const lock(_mutex);
        for (auto const &a : _addons) {
            if (a.handler->handles(k))
                return &a;
        }
        return nullptr;
    }

    bool loadPlugin(QString const &fileName) {
        std::lock_guard<std::recursive_mutex> const lock(_mutex);
        _error.clear();

        QLibrary library(fileName);
        if (!library.load()) {
            _error = QObject::tr("Cannot load '%1': %2").arg(fileName, library.errorString());
            return false;
        }

        if (!checkAddonVersion(library, _error))
            return false;
        Interface *addon = createInterface(library);
        if (!addon) {
            _error = QStringLiteral("'%1' did not create the required addon").arg(fileName);
            return false;
        }

        _addons.insert(unique_ptr<Interface>(addon));
        return true;
    }

    int loadPlugins(QString const &directory) {
        int loaded = 0;
        QFileInfoList const entries = QDir(directory).entryInfoList(QDir::Files);
        for (QFileInfo const &info : entries) {
            QString const file = info.absoluteFilePath();
            if (QLibrary::isLibrary(file) && loadPlugin(file))
                ++loaded;
        }
        return loaded;
    }

    QString error() const {
        std::lock_guard<std::recursive_mutex> const lock(_mutex);
        return _error;
    }

protected:
    AddonRegistry() = default;

private:
    GREU_NO_COPY(AddonRegistry);
    GREU_NO_MOVE(AddonRegistry);

    mutable std::recursive_mutex _mutex;
    Addons _addons;
    QString _error;
    virtual bool checkAddonVersion(QLibrary &, QString const &fileName, QString &error) = 0;
    virtual Interface *createInterface(QLibrary &) = 0;
};

} // namespace greu::core
