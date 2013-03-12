#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/gfx/DX9/directx.hpp"
#include "tools/gfx/DX9/FragmentProgram.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    FragmentProgram::FragmentProgram(DX9Renderer& renderer, std::string const& code) :
        _renderer(renderer)
    {
        // Compilation:
        ID3DXBuffer* errors;
        ID3DXBuffer* psShader;
        ID3DXConstantTable* constants;
        if (FAILED(D3DXCompileShader(code.c_str(), (UINT)code.size(), nullptr, nullptr, "main", "ps_3_0", D3DXSHADER_OPTIMIZATION_LEVEL3 | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, &psShader, &errors, &constants)))
            throw std::runtime_error(std::string("DX9::Shader errors: ") + (char const*)errors->GetBufferPointer());

        IDirect3DPixelShader9* tmpPS;
        DXCHECKERROR(this->_renderer.GetDevice()->CreatePixelShader((DWORD*)psShader->GetBufferPointer(), &tmpPS));
        this->_shader.reset(tmpPS);
        this->_constants.reset(constants);
    }

}}}

#endif
