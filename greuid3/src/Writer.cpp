#include "Writer.h"
#include "Frame.h"
#include "FrameHandler.h"
#include "FrameHandlerRegistry.h"
#include "Tag.h"
#include "greucore/include/intUtils.h"

namespace {
static uchar constexpr zero = 0x00;
} // namespace

namespace greu::id3 {

using namespace core;

QByteArray Writer::write(Tag const &tag) const {
    FrameHandlerRegistry &registry = FrameHandlerRegistry::instance();
    FrameHandler::RenderedFrames frames;

    for (auto &addon : registry.addons())
        for (auto const &a : tag.attributes()) {
            if (!a.second.attribute->isEmpty())
                try {
                    frames.append_range(addon.handler->render(*a.second.attribute));
                } catch (...) {
                    continue;
                }
        }
    auto const additional = tag.additionalFrames();
    for (auto const &frame : additional)
        frames.push_back({frame.id(), {frame.data()}});

    if (frames.empty())
        return QByteArray();

    QByteArray body;
    for (auto const &frame : frames) {
        if (frame.data.size() > 0) {
            for (auto const &f : frame.data) {
                auto f1 = f.size();
                auto fff1 = toSyncBytes32(f1);
                body.append(encode(frame.id, MibNumber::Latin1)
                                .append(toSyncBytes32(f.size()).append(zero).append(zero))
                                .append(f));
            }
        }
    }

    QByteArray out;
    out.append("ID3", 3);
    out.append('\x04'); // major version
    out.append('\0');   // revision
    out.append('\0');   // flags
    out.append(toSyncBytes32(static_cast<quint32>(body.size())));
    out.append(body);
    return out;
}

} // namespace greu::id3
