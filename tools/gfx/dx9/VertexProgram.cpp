#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/gfx/DX9/directx.hpp"
#include "tools/gfx/DX9/VertexProgram.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    VertexProgram::VertexProgram(DX9Renderer& renderer, std::string const& code) :
        _renderer(renderer)
    {
        // Compilation:
        ID3DXBuffer* errors;
        ID3DXBuffer* vsShader;
        ID3DXConstantTable* constants;
        if (FAILED(D3DXCompileShader(code.c_str(), (UINT)code.size(), nullptr, nullptr, "main", "vs_3_0", D3DXSHADER_OPTIMIZATION_LEVEL3 | D3DXSHADER_ENABLE_BACKWARDS_COMPATIBILITY, &vsShader, &errors, &constants)))
            throw std::runtime_error(std::string("DX9::Shader errors: ") + (char const*)errors->GetBufferPointer());

        IDirect3DVertexShader9* tmp;
        DXCHECKERROR(this->_renderer.GetDevice()->CreateVertexShader((DWORD*)vsShader->GetBufferPointer(), &tmp));
        this->_shader.reset(tmp);
        this->_constants.reset(constants);
    }

}}}

#endif
