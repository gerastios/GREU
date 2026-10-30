#pragma once

#include <QCoreApplication>
#include <QObject>
#include "greuGlobal.h"
#include <memory>
#include <QPointer>

namespace greu::core {

template<typename T>
struct GREU_PUBLIC(CORE) GuardedDeleter {
    GuardedDeleter() {};
    GuardedDeleter(GuardedDeleter const &) = default;
    GuardedDeleter(GuardedDeleter &) = default;
    GuardedDeleter(GuardedDeleter &&) = default;
    void operator()(T *p) const {
        if (!p || !_qp)
            return;
        if constexpr (std::is_base_of_v<QObject, T>) {
            if (!QCoreApplication::instance()) {
                delete p;
                return;
            }
            p->deleteLater(); // no RTTI, no virtual dispatch
        } else if constexpr (std::is_polymorphic_v<T>) {
            if (auto *qt = dynamic_cast<QObject *>(p)) {
                if (!QCoreApplication::instance()) {
                    delete p;
                    return;
                }
                qt->deleteLater();
                return;
            }
            delete p;
        } else {
            delete p;
        }
    };
    void setPointer(QPointer<T> qp) { _qp = qp; }

private:
    QPointer<T> _qp;
};

template<typename T>
struct GREU_PUBLIC(CORE) GuardedPtr : public std::unique_ptr<T, GuardedDeleter<T>> {
public:
    GuardedPtr()
        : std::unique_ptr<T, GuardedDeleter<T>>() {}
    GuardedPtr(T *p)
        : std::unique_ptr<T, GuardedDeleter<T>>(p, GuardedDeleter<T>()) {
        static_cast<GuardedDeleter<T>>(this->get_deleter()).setPointer(QPointer(p));
    }
    GuardedPtr(GuardedPtr<T> &&other) { this->reset(other.release()); }
    GuardedPtr &operator=(GuardedPtr &&other) {
        this->reset(other.release());
        return *this;
    }
};

} // namespace greu::core
