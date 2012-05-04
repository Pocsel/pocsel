#include "tools/precompiled.hpp"

#include "tools/Color.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Matrix4.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/Texture2D.hpp"
#include "tools/renderers/opengl/ShaderProgramCg.hpp"
#include "tools/renderers/opengl/ShaderParameterCg.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    ShaderParameterCg::ShaderParameterCg(ShaderProgramCg& program, std::string const& identifier)
    {
        this->_param = cgGetNamedEffectParameter(program.GetEffect(), identifier.c_str());
    }

    ShaderParameterCg::ShaderParameterCg(ShaderProgramCg& program, CGparameter parameter) :
        _param(parameter)
    {
    }

    ShaderParameterCg::~ShaderParameterCg()
    {
        if (this->_param != 0)
            cgDestroyParameter(this->_param);
    }

    void ShaderParameterCg::Set(bool value)
    {
        cgSetParameter1i(this->_param, value ? 1 : 0);
    }

    void ShaderParameterCg::Set(int value)
    {
        cgSetParameter1i(this->_param, value);
    }

    void ShaderParameterCg::Set(float value)
    {
        cgSetParameter1f(this->_param, value);
    }

    void ShaderParameterCg::Set(Color4<float> const& color)
    {
        cgSetParameter4fv(this->_param, color.colors);
    }

    void ShaderParameterCg::Set(glm::detail::tvec2<float> const& vector)
    {
        cgSetParameter2fv(this->_param, (float*)&vector);
    }

    void ShaderParameterCg::Set(glm::detail::tvec3<float> const& vector)
    {
        cgSetParameter3fv(this->_param, (float*)&vector);
    }

    void ShaderParameterCg::Set(glm::detail::tmat4x4<float> const& matrix, bool)
    {
        cgSetMatrixParameterfc(this->_param, (float*)&matrix);
    }

    void ShaderParameterCg::Set(std::vector<glm::mat4x4> const& matrices)
    {
        if (!this->_preBuffer)
        {
            this->_preBuffer.reset(new std::vector<float>(
                    cgGetParameterColumns(this->_param) *
                    cgGetParameterRows(this->_param) *
                    cgGetArrayTotalSize(this->_param)
                    ));
        }

        std::memcpy(this->_preBuffer->data(), matrices.data(), sizeof(float) * std::min(this->_preBuffer->size(), matrices.size() * 4*4));
        cgSetParameterValuefc(this->_param, (int)this->_preBuffer->size(), this->_preBuffer->data());
    }

    void ShaderParameterCg::Set(ITexture2D& texture)
    {
        Texture2D& tex = reinterpret_cast<Texture2D&>(texture);
        //assert(tex.GetBindID() >= 0 && "Faut bind la texture avant.");
        cgGLSetTextureParameter(this->_param, tex.GetID());
        cgSetSamplerState(this->_param);
    }

}}}
