#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/DX9Renderer.hpp"
#include "tools/gfx/dx9/directx.hpp"
#include "tools/gfx/dx9/Texture2D.hpp"
#include "tools/gfx/dx9/Program.hpp"
#include "tools/gfx/dx9/ShaderParameter.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    ShaderParameter::ShaderParameter(Program& program, std::string const& identifier) :
        _device(program.GetRenderer().GetDevice())
    {
        this->_vertexConstants = program.GetVertexProgram().GetConstants();
        this->_vertexParam = this->_vertexConstants->GetConstantByName(nullptr, identifier.c_str());

        this->_fragmentConstants = program.GetFragmentProgram().GetConstants();
        this->_fragmentParam = this->_fragmentConstants->GetConstantByName(nullptr, identifier.c_str());

        if (this->_vertexParam == nullptr && this->_fragmentParam == nullptr)
            throw std::runtime_error("ShaderParameter: " + identifier + " didn't found");

        if (this->_fragmentParam != nullptr)
        {
            UINT count = 1;
            DXCHECKERROR(this->_fragmentConstants->GetConstantDesc(this->_fragmentParam, &this->_desc, &count));
        }
    }

    //ShaderParameter::ShaderParameter(Program& program, D3DXHANDLE parameter) :
    //    _vertexParam(parameter)
    //{
    //}

    ShaderParameter::~ShaderParameter()
    {
    }

    void ShaderParameter::Set(int value)
    {
        if (this->_vertexParam != nullptr)
            DXCHECKERROR(this->_vertexConstants->SetInt(this->_device, this->_vertexParam, value));
        if (this->_fragmentParam != nullptr)
            DXCHECKERROR(this->_fragmentConstants->SetInt(this->_device, this->_fragmentParam, value));
    }

    void ShaderParameter::Set(float value)
    {
        if (this->_vertexParam != nullptr)
            DXCHECKERROR(this->_vertexConstants->SetFloat(this->_device, this->_vertexParam, value));
        if (this->_fragmentParam != nullptr)
            DXCHECKERROR(this->_fragmentConstants->SetFloat(this->_device, this->_fragmentParam, value));
    }

    void ShaderParameter::Set(glm::detail::tvec2<float> const& vector)
    {
        if (this->_vertexParam != nullptr)
            DXCHECKERROR(this->_vertexConstants->SetFloatArray(this->_device, this->_vertexParam, (float const*)&vector, 2));
        if (this->_fragmentParam != nullptr)
            DXCHECKERROR(this->_fragmentConstants->SetFloatArray(this->_device, this->_fragmentParam, (float const*)&vector, 2));
    }

    void ShaderParameter::Set(glm::detail::tvec3<float> const& vector)
    {
        if (this->_vertexParam != nullptr)
            DXCHECKERROR(this->_vertexConstants->SetFloatArray(this->_device, this->_vertexParam, (float const*)&vector, 3));
        if (this->_fragmentParam != nullptr)
            DXCHECKERROR(this->_fragmentConstants->SetFloatArray(this->_device, this->_fragmentParam, (float const*)&vector, 3));
    }

    void ShaderParameter::Set(glm::detail::tvec4<float> const& vector)
    {
        if (this->_vertexParam != nullptr)
            DXCHECKERROR(this->_vertexConstants->SetFloatArray(this->_device, this->_vertexParam, (float const*)&vector, 4));
        if (this->_fragmentParam != nullptr)
            DXCHECKERROR(this->_fragmentConstants->SetFloatArray(this->_device, this->_fragmentParam, (float const*)&vector, 4));
    }

    void ShaderParameter::Set(glm::detail::tmat4x4<float> const& matrix, bool isProjection)
    {
        if (isProjection)
        {
            auto tmp = DX9Renderer::glToDirectX * matrix;
            if (this->_vertexParam != nullptr)
                DXCHECKERROR(this->_vertexConstants->SetMatrixTranspose(this->_device, this->_vertexParam, (D3DXMATRIX const*)&tmp));
            if (this->_fragmentParam != nullptr)
                DXCHECKERROR(this->_fragmentConstants->SetMatrixTranspose(this->_device, this->_fragmentParam, (D3DXMATRIX const*)&tmp));
        }
        else
        {
            if (this->_vertexParam != nullptr)
                DXCHECKERROR(this->_vertexConstants->SetMatrixTranspose(this->_device, this->_vertexParam, (D3DXMATRIX const*)&matrix));
            if (this->_fragmentParam != nullptr)
                DXCHECKERROR(this->_fragmentConstants->SetMatrixTranspose(this->_device, this->_fragmentParam, (D3DXMATRIX const*)&matrix));
        }
    }

    void ShaderParameter::Set(std::vector<glm::mat4x4> const& matrices)
    {
        if (this->_vertexParam != nullptr)
            DXCHECKERROR(this->_vertexConstants->SetMatrixTransposeArray(this->_device, this->_vertexParam, (D3DXMATRIX const*)matrices.data(), (UINT)matrices.size()));
        if (this->_fragmentParam != nullptr)
            DXCHECKERROR(this->_fragmentConstants->SetMatrixTransposeArray(this->_device, this->_fragmentParam, (D3DXMATRIX const*)matrices.data(), (UINT)matrices.size()));
    }

    void ShaderParameter::Set(ITexture2D& texture)
    {
        assert(this->_fragmentParam != 0 && "Ce n'est pas une texture");
        Texture2D& tex = reinterpret_cast<Texture2D&>(texture);
        //assert(tex.GetBindID() >= 0 && "Faut bind la texture avant.");
        //DXCHECKERROR(this->_effect->SetTexture(this->_vertexParam, tex.GetTexture()));
        //DXCHECKERROR(this->_effect->SetValue(this->_vertexParam, tex.GetTexture(), sizeof(void*)));
        DXCHECKERROR(this->_device->SetTexture(this->_desc.RegisterIndex, tex.GetTexture()));
    }

    void ShaderParameter::Set(ITexture2D& texture, ISamplerState& sampler)
    {
        assert(this->_fragmentParam != 0 && "Ce n'est pas une texture");
        Texture2D& tex = reinterpret_cast<Texture2D&>(texture);
        sampler.Bind(this->_desc.RegisterIndex);
        DXCHECKERROR(this->_device->SetTexture(this->_desc.RegisterIndex, tex.GetTexture()));
    }

}}}

#endif