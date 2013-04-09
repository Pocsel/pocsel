#pragma once

#include "tools/gfx/effect/RenderState.hpp"
#include "tools/gfx/effect/Shader.hpp"
#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace Effect {

    class DummyShaderParameter : public Tools::Gfx::IShaderParameter
    {
    public:
        virtual void Set(int) {}
        virtual void Set(float) {}
        virtual void Set(glm::vec2 const&) {}
        virtual void Set(glm::vec3 const&) {}
        virtual void Set(glm::vec4 const&) {}
        virtual void Set(glm::mat4 const&, bool) {}
        virtual void Set(std::vector<glm::mat4x4> const&) {}
        virtual void Set(ITexture2D&) {}
        virtual void Set(ITexture2D&, ISamplerState&) {}

        virtual bool IsUseable() const { return false; }
    };

    class TextureShaderParameter : public Tools::Gfx::IShaderParameter
    {
    private:
        IShaderParameter& _parameter;
        std::unique_ptr<ISamplerState> _sampler;

    public:
        TextureShaderParameter(IShaderParameter& param, std::unique_ptr<ISamplerState>&& sampler);
        virtual void Set(int) { throw std::runtime_error("not supported"); }
        virtual void Set(float) { throw std::runtime_error("not supported"); }
        virtual void Set(glm::vec2 const&) { throw std::runtime_error("not supported"); }
        virtual void Set(glm::vec3 const&) { throw std::runtime_error("not supported"); }
        virtual void Set(glm::vec4 const&) { throw std::runtime_error("not supported"); }
        virtual void Set(glm::mat4 const&, bool) { throw std::runtime_error("not supported"); }
        virtual void Set(std::vector<glm::mat4x4> const&) { throw std::runtime_error("not supported"); }
        virtual bool IsUseable() const { return true; }

        virtual void Set(ITexture2D& tex);
        virtual void Set(ITexture2D& tex, ISamplerState& sampler);
    };

    class Effect
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IProgram> _program;
        Shader _shader;
        RenderState _state;
        std::map<std::string, std::unique_ptr<TextureShaderParameter>> _textureParameters;
        std::map<std::string, std::unique_ptr<DummyShaderParameter>> _dummyParameters;

    public:
        Effect(IRenderer& renderer, CompleteShader const& shader);

        IShaderParameter& GetParameter(std::string const& identifier);
        //IShaderParameter& GetParameterFromSemantic(std::string const& identifier) { return this->_program->GetParameterFromSemantic(identifier); }
        void BeginPass() { this->_shader.renderState.Set(this->_renderer); this->_program->Begin(); }
        bool EndPass() { this->_program->End(); return false; }
    };

}}}
