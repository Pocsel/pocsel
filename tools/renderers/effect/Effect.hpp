#pragma once

#include "tools/IRenderer.hpp"
#include "tools/renderers/effect/RenderState.hpp"

namespace Tools { namespace Renderers { namespace Effect {

    class CompleteShader;

    class Effect
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IProgram> _program;
        RenderState _state;

    public:
        Effect(IRenderer& renderer, CompleteShader const& shader);

        IShaderParameter& GetParameter(std::string const& identifier) { return this->_program->GetParameter(identifier); }
        //IShaderParameter& GetParameterFromSemantic(std::string const& identifier) { return this->_program->GetParameterFromSemantic(identifier); }
        void BeginPass() { this->_program->Begin(); }
        bool EndPass() { this->_program->End(); return false; }
    };

}}}
