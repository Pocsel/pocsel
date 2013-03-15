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

        virtual bool IsUseable() const { return false; }
    };

    class Effect
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IProgram> _program;
        Shader _shader;
        RenderState _state;
        std::map<std::string, std::unique_ptr<DummyShaderParameter>> _parameters;

    public:
        Effect(IRenderer& renderer, CompleteShader const& shader);

        IShaderParameter& GetParameter(std::string const& identifier);
        //IShaderParameter& GetParameterFromSemantic(std::string const& identifier) { return this->_program->GetParameterFromSemantic(identifier); }
        void BeginPass() { this->_program->Begin(); }
        bool EndPass() { this->_program->End(); return false; }
    };

}}}
