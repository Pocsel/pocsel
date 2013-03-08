#pragma once

#include "tools/gfx/effect/RenderState.hpp"
#include "tools/gfx/effect/Shader.hpp"
#include "tools/gfx/IRenderer.hpp"

namespace Tools { namespace Gfx { namespace Effect {

    class Effect
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IProgram> _program;
        Shader _shader;
        RenderState _state;

    public:
        Effect(IRenderer& renderer, CompleteShader const& shader);

        IShaderParameter& GetParameter(std::string const& identifier);
        //IShaderParameter& GetParameterFromSemantic(std::string const& identifier) { return this->_program->GetParameterFromSemantic(identifier); }
        void BeginPass() { this->_program->Begin(); }
        bool EndPass() { this->_program->End(); return false; }
    };

}}}
