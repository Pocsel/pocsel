#pragma once

#include "tools/gfx/DX9Renderer.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    class FragmentProgram : public IFragmentProgram
    {
    private:
        DX9Renderer& _renderer;
        ComPtr<IDirect3DPixelShader9> _shader;
        ComPtr<ID3DXConstantTable> _constants;

    public:
        FragmentProgram(FragmentProgram&& prg) : _renderer(prg._renderer), _shader(std::move(prg._shader)), _constants(std::move(prg._constants)) {}
        FragmentProgram(DX9Renderer& renderer, std::string const& code);

        IDirect3DPixelShader9* GetShader() const { return this->_shader.get(); }
        ID3DXConstantTable* GetConstants() const { return this->_constants.get(); }

    private:
        FragmentProgram(FragmentProgram const&);
        FragmentProgram& operator =(FragmentProgram const&);
    };

}}}
