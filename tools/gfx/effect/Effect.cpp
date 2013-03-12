#include "tools/precompiled.hpp"

#include "tools/gfx/effect/Effect.hpp"
#include "tools/gfx/effect/Shader.hpp"

namespace Tools { namespace Gfx { namespace Effect {

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

        ShaderParameterUsage::Type GetShaderParameterUsage(Semantic::Type semantic)
        {
            switch (semantic)
            {
            case Semantic::Projection: return ShaderParameterUsage::ProjectionMatrix;
            case Semantic::View: return ShaderParameterUsage::ViewMatrix;
            case Semantic::World: return ShaderParameterUsage::ModelMatrix;
            case Semantic::WorldView: return ShaderParameterUsage::ModelViewMatrix;
            case Semantic::WorldViewProjection: return ShaderParameterUsage::ModelViewProjectionMatrix;
            case Semantic::ViewProjection: return ShaderParameterUsage::ViewProjectionMatrix;
            }
            return ShaderParameterUsage::None;
        }
    }

    Effect::Effect(IRenderer& renderer, CompleteShader const& complShader) :
        _renderer(renderer)
    {
        this->_shader = renderer.GetRendererName().find("DirectX") != -1 ? complShader.directX : complShader.openGL;
        this->_program = renderer.CreateProgram(this->_shader.vertex, this->_shader.pixel);
        for(auto const& pair: this->_shader.attributes)
            this->_program->SetAttributeUsage(pair.second.name, GetAttributeUsage(pair.second.semantic));
        for (auto const& pair: this->_shader.uniforms)
        {
            auto usage = GetShaderParameterUsage(pair.second.semantic);
            if (usage != ShaderParameterUsage::None)
                this->_program->SetParameterUsage(usage, pair.second.name);
        }
    }

    IShaderParameter& Effect::GetParameter(std::string const& identifier)
    {
        auto const& it = this->_shader.uniforms.find(identifier);
        if (it == this->_shader.uniforms.end())
            throw std::invalid_argument(identifier + " not found");
        return this->_program->GetParameter(it->second.name);
    }

}}}
