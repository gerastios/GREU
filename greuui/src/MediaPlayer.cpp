#include "MediaPlayer.h"
#include <QAudioOutput>
#include <greucore/include/GuardedPtr.h>

namespace {
greu::core::GuardedPtr<QAudioOutput> out;
}

namespace greu::ui {

MediaPlayer::MediaPlayer() {
    if(!out)
        out = new QAudioOutput;
    setAudioOutput(out.get());
}

} // namespace greu::ui
