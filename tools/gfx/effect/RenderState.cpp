#include "tools/precompiled.hpp"

#include "tools/ByteArray.hpp"
#include "tools/gfx/effect/RenderState.hpp"

namespace Tools { namespace Gfx { namespace Effect {

    RenderState::RenderState() :
        zEnabled(true),
        zWriteEnabled(true),
        zFunc(ZFunc::Less),
        fillMode(RasterizationMode::Fill),
        alphaTestEnable(true),
        blendSrc(Blend::One),
        blendDest(Blend::Zero),
        cullMode(CullMode::CounterClockwise)
    {
    }

}}}
