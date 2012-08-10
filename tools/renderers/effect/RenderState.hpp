#ifndef __TOOLS_RENDERERS_EFFECT_RENDERSTATE_HPP__
#define __TOOLS_RENDERERS_EFFECT_RENDERSTATE_HPP__

#include "tools/ByteArray.hpp"

namespace Tools { namespace Renderers { namespace Effect {

    namespace ZFunc
    {
        enum Type
        {

        };
    }

    namespace Blend
    {
        enum Type
        {
            Zero,
            One,
            SrcColor,
            InvSrcColor,
            SrcAlpha,
            InvSrcAlpha,
            DestAlpha,
            InvDestAlpha,
            DestColor,
            InvDestColor,
            SrcAlphaSat,
            BothSrcAlpha,
            BothInvSrcAlpha,
            BlendFactor,
            InvBlendFactor,
            SrcColor2,
            InvSrcColor2
        };
    }

    struct RenderState
    {
        bool zEnabled;
        bool zWriteEnabled;
        ZFunc::Type zFunc;
        RasterizationMode::Type fillMode;
        bool alphaTestEnable;
        Blend::Type srcBlend;
        Blend::Type destBlend;
        CullMode::Type cullMode;

        RenderState(ByteArray& stream);
    };

}}}

#endif
