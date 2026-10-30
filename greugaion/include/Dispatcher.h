#pragma once
#include "greuui/include/Dispatcher.h"
#include <boost/signals2.hpp>

namespace greu::movies {
class Collection;
class File;
} // namespace greu::movies

namespace greu::gaion {

class GREU_PUBLIC(GAION) Dispatcher : public ui::Dispatcher {
public:
    Dispatcher();
    virtual ~Dispatcher() = default;

    virtual void initialize() override {};

    void back();
    void forward();
    void linkClicked(QString const &);

    using FileSig = boost::signals2::signal<void(movies::File *)>;
    FileSig &playingFinished() const;

    enum class PlayType {
        play,
        continuePlaying,
        playExtended,
    };
    void play(movies::File *, PlayType);

    void exit();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::gaion
