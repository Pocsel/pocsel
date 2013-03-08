#include "tools/precompiled.hpp"

#include "tools/ByteArray.hpp"
#include "tools/renderers/effect/RenderState.hpp"

namespace Tools { namespace Renderers { namespace Effect {

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
