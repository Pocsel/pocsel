#pragma once

#include "tools/gfx/DX9Renderer.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    class VertexProgram : public IVertexProgram
    {
    private:
        DX9Renderer& _renderer;
        ComPtr<IDirect3DVertexShader9> _shader;
        ComPtr<ID3DXConstantTable> _constants;

    public:
        VertexProgram(VertexProgram&& prg) : _renderer(prg._renderer), _shader(std::move(prg._shader)), _constants(std::move(prg._constants)) {}
        VertexProgram(DX9Renderer& renderer, std::string const& code);

        IDirect3DVertexShader9* GetShader() const { return this->_shader.get(); }
        ID3DXConstantTable* GetConstants() const { return this->_constants.get(); }

    private:
        VertexProgram(VertexProgram const&);
        VertexProgram& operator =(VertexProgram const&);
    };

}}}
