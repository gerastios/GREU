#pragma once

#include <QFileInfo>
#include <QString>
#include "greucore/include/greuGlobal.h"
#include <any>

namespace greu::core {
class Log;
}

namespace greu::tag {
class Attribute;
}
namespace greu::id3 {

class Tag;
class Frame;

struct GREU_PUBLIC(ID3) FrameHandler {
    using ParseData = std::unordered_map<QString, std::any>;
    struct RenderedFrame {
        QString id;
        std::vector<QByteArray> data;
    };
    using RenderedFrames = std::vector<RenderedFrame>;

    virtual bool handles(QString const &) const { return true; };
    inline virtual void beginTag(ParseData &) {};
    inline virtual void endTag(ParseData &, greu::id3::Tag *) {};
    virtual bool parse(Tag *, Frame const *, core::Log *, ParseData &) const = 0;
    virtual RenderedFrames render(tag::Attribute const &) const = 0;
};

} // namespace greu::id3

#define GREU_ID3_FRAME_HANDLER_PLUGIN_API_VERSION 1

#define GREU_ID3_FRAME_HANDLER_PLUGIN_API_VERSION_SYMBOL "greu_tag_file_creator_plugin_api_version"
#define GREU_ID3_FRAME_HANDLER_PLUGIN_CREATE_SYMBOL "greu_tag_create_file"

#if defined(Q_OS_WIN) || defined(_WIN32)
#define GREU_ID3_FRAME_HANDLER_EXPORT __declspec(dllexport)
#else
#define GREU_ID3_FRAME_HANDLER_EXPORT __attribute__((visibility("default")))
#endif

extern "C" {
typedef int (*Id3FrameHandlerApiVersionFunction)();
typedef greu::id3::FrameHandler *(*Id3FrameHandlerFunction)();
}
