#pragma once

#include "tools/gfx/IRenderer.hpp"

namespace Tools {
    class ByteArray;
}

namespace Tools { namespace Gfx { namespace Effect {

    struct Shader;

    struct RenderState
    {
        CullMode::Type cullMode;
        bool alphaBlendEnable;
        AlphaFunc::Type alphaFunc;
        float alphaRef;
        bool alphaTestEnable;
        Blend::Type srcBlend;
        Blend::Type destBlend;
        BlendOp::Type blendOp;
        Blend::Type srcBlendAlpha;
        Blend::Type destBlendAlpha;
        BlendOp::Type blendOpAlpha;
        bool ditherEnable;
        FillMode::Type fillMode;
        bool zEnable;
        ZFunc::Type zFunc;
        bool zWriteEnable;

        RenderState();

        void Load(Shader& shader);
        void Set(IRenderer& renderer);
    };

}}}
