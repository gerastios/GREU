#pragma once
#include "FrameHandler.h"

namespace greu::id3 {

class GREU_PUBLIC(ID3) DefaultFrameHandler : public FrameHandler {
public:
    virtual void beginTag(ParseData &) override;
    virtual void endTag(ParseData &, Tag *) override;
    virtual bool parse(Tag *, Frame const *, core::Log *, ParseData &) const override;
    virtual RenderedFrames render(tag::Attribute const &) const override;
};

} // namespace greu::Id3
