#ifdef _WIN32
#include "tools/precompiled.hpp"

#include "tools/Color.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Matrix4.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"
#include "tools/renderers/DX9Renderer.hpp"
#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/Texture2D.hpp"
#include "tools/renderers/dx9/ShaderProgram.hpp"
#include "tools/renderers/dx9/ShaderParameter.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    ShaderParameter::ShaderParameter(ShaderProgram& program, std::string const& identifier)
        : _effect(program.GetEffect())
    {
        this->_param = this->_effect->GetParameterByName(0, identifier.c_str());
    }

    ShaderParameter::ShaderParameter(ShaderProgram& program, D3DXHANDLE parameter)
        : _effect(program.GetEffect()),
        _param(parameter)
    {
    }

    ShaderParameter::~ShaderParameter()
    {
    }

    void ShaderParameter::Set(bool value)
    {
        this->_effect->SetBool(this->_param, value ? TRUE : FALSE);
    }

    void ShaderParameter::Set(int value)
    {
        this->_effect->SetInt(this->_param, value);
    }

    void ShaderParameter::Set(float value)
    {
        this->_effect->SetFloat(this->_param, value);
    }

    void ShaderParameter::Set(Color4<float> const& color)
    {
        this->_effect->SetFloatArray(this->_param, color.colors, 4);
    }

    void ShaderParameter::Set(glm::detail::tvec2<float> const& vector)
    {
        this->_effect->SetFloatArray(this->_param, (float const*)&vector, 2);
    }

    void ShaderParameter::Set(glm::detail::tvec3<float> const& vector)
    {
        this->_effect->SetFloatArray(this->_param, (float const*)&vector, 3);
    }

    void ShaderParameter::Set(glm::detail::tvec4<float> const& vector)
    {
        this->_effect->SetFloatArray(this->_param, (float const*)&vector, 4);
    }

    void ShaderParameter::Set(glm::detail::tmat4x4<float> const& matrix, bool isProjection)
    {
        if (isProjection)
        {
            auto tmp = DX9Renderer::glToDirectX * matrix;
            this->_effect->SetMatrixTranspose(this->_param, (D3DXMATRIX const*)&tmp);
        }
        else
            this->_effect->SetMatrixTranspose(this->_param, (D3DXMATRIX const*)&matrix);
    }

    void ShaderParameter::Set(std::vector<glm::mat4x4> const& matrices)
    {
        this->_effect->SetMatrixTransposeArray(this->_param, (D3DXMATRIX const*)matrices.data(), (UINT)matrices.size());
    }

    void ShaderParameter::Set(ITexture2D& texture)
    {
        Texture2D& tex = reinterpret_cast<Texture2D&>(texture);
        //assert(tex.GetBindID() >= 0 && "Faut bind la texture avant.");
        DXCHECKERROR(this->_effect->SetTexture(this->_param, tex.GetTexture()));
    }

}}}

#endif