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

    void ShaderParameterCg::Set(Vector2<float> const& vector)
    {
        cgSetParameter2fv(this->_param, vector.coords);
    }

    void ShaderParameterCg::Set(Vector3<float> const& vector)
    {
        cgSetParameter3fv(this->_param, vector.coords);
    }

    void ShaderParameterCg::Set(Matrix4<float> const& matrix)
    {
        cgSetMatrixParameterfc(this->_param, matrix.mm);
    }

    void ShaderParameterCg::Set(ITexture2D& texture)
    {
        Texture2D& tex = reinterpret_cast<Texture2D&>(texture);
        //assert(tex.GetBindID() >= 0 && "Faut bind la texture avant.");
        cgGLSetTextureParameter(this->_param, tex.GetID());
        cgSetSamplerState(this->_param);
    }

}}}
