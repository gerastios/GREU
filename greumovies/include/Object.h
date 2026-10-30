#pragma once
#include <QString>
#include "ObjectState.h"
#include "greucore/include/Uuid.h"
#include "greucore/include/utils.h"

namespace greu::movies {

class GREU_PUBLIC(MOVIES) Object : public ObjectState {
public:
    virtual ~Object() = default;

    template<class T>
    static T *object(core::Uuid const &u) {
        auto o = existingObject(u);
        if (o)
            return core::safeCast<T>(o);
        auto no = std::unique_ptr<T>(new T{u});
        no->read();
        auto res = no.get();
        addObject(u, std::move(no));
        return res;
    }

    static Object *existingObject(core::Uuid const &u);

    static bool exists(core::Uuid const &);

    GREU_NO_COPY(Object);
    GREU_NO_MOVE(Object);

    core::Uuid const &uuid() const;

    virtual void save() = 0;
    static void remove(core::Uuid const &);

protected:
    explicit Object(core::Uuid const &);

private:
    struct Detail;
    core::DetailPtr<Detail> d;
    static void addObject(core::Uuid const &, std::unique_ptr<Object>);

    virtual void read() = 0;
};

} // namespace greu::movies
