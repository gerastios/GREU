#include "Time.h"

namespace greu::core {

Time::Time(int h, int m, int s, int ms)
    : QTime(h, m, s, ms) {}

Time Time::fromSeconds(uint seconds) {
    int h = seconds / 3600;
    int m = (seconds - h * 3600) / 60;
    int s = (seconds - h * 3600 - m * 60);
    return Time{h, m, s};
}

} // namespace greu::core
