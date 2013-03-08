#include "tools/precompiled.hpp"

#include "tools/renderers/effect/Effect.hpp"
#include "tools/renderers/effect/Shader.hpp"

namespace Tools { namespace Renderers { namespace Effect {

    namespace {
        VertexAttributeUsage::Type GetAttributeUsage(Semantic::Type semantic)
        {
            switch (semantic)
            {
            case Semantic::Position: return VertexAttributeUsage::Position;
            case Semantic::Normal: return VertexAttributeUsage::Normal;
            case Semantic::Color: return VertexAttributeUsage::Color;
            case Semantic::TexCoord0: return VertexAttributeUsage::TexCoord0;
            case Semantic::TexCoord1: return VertexAttributeUsage::TexCoord1;
            case Semantic::TexCoord2: return VertexAttributeUsage::TexCoord2;
            case Semantic::TexCoord3: return VertexAttributeUsage::TexCoord3;
            case Semantic::TexCoord4: return VertexAttributeUsage::TexCoord4;
            }
            throw std::invalid_argument("it's not an attribute");
        }
    }

    Effect::Effect(IRenderer& renderer, CompleteShader const& complShader) :
        _renderer(renderer)
    {
        Shader shader = renderer.GetRendererName() == "DirectX" ? complShader.directX : complShader.openGL;
        this->_program = renderer.CreateProgram(shader.vertex, shader.pixel);
        for(auto const& pair: shader.attributes)
            this->_program->SetAttributeUsage(pair.second.name, GetAttributeUsage(pair.second.semantic));

    }

}}}
