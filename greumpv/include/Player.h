#pragma once
#include <QFileInfo>
#include "greucore/include/DetailPtr.h"
#include "greucore/include/signalUtils.h"

namespace greu::mpv {

class GREU_PUBLIC(MPV) Player {
public:
    Player(QFileInfo const &);
    GREU_NO_COPY(Player)
    GREU_NO_MOVE(Player)

    void play(double position = 0, uint aid = 0, uint sid = 0, int chapter = 0, int64_t volume = 100);
    double position() const;
    uint aid() const;
    uint sid() const;
    uint volume() const;

    core::EmptySig &finished() const;
    core::EmptySig &posChanged() const;

    void stop();

private:
    struct Detail;
    core::DetailPtr<Detail> d;
};

} // namespace greu::mpv
