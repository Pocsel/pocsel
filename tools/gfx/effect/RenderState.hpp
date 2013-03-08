#pragma once

#include "tools/gfx/IRenderer.hpp"

namespace Tools {
    class ByteArray;
}

namespace Tools { namespace Gfx { namespace Effect {

    namespace ZFunc
    {
        enum Type
        {
            Never,
            Less,
            LessEqual,
            Equal,
            NotEqual,
            Greater,
            GreaterEqual,
            Always
        };
    }

    namespace Blend
    {
        enum Type
        {
            Zero,
            One,
            DestColor,
            OneMinusDestColor,
            InvDestColor,
            SrcAlpha,
            OneMinusSrcAlpha,
            InvSrcAlpha,
            DstAlpha,
            OneMinusDstAlpha,
            InvDestAlpha,
            SrcAlphaSaturate,
            SrcAlphaSat,
            SrcColor,
            OneMinusSrcColor,
            InvSrcColor,
            ConstantColor,
            BlendFactor,
            OneMinusConstantColor,
            InvBlendFactor,
            ConstantAlpha,
            OneMinusConstantAlpha
        };
    }

    struct RenderState
    {
        bool zEnabled;
        bool zWriteEnabled;
        ZFunc::Type zFunc;
        RasterizationMode::Type fillMode;
        bool alphaTestEnable;
        Blend::Type blendSrc;
        Blend::Type blendDest;
        CullMode::Type cullMode;

        RenderState();
    };

}}}
