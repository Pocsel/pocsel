#include "tools/precompiled.hpp"

#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

#include "tools/gfx/IRenderer.hpp"
#include "tools/gfx/opengl/opengl.hpp"
#include "tools/gfx/opengl/Texture2D.hpp"
#include "tools/gfx/opengl/Program.hpp"
#include "tools/gfx/opengl/ShaderParameter.hpp"

namespace Tools { namespace Gfx { namespace OpenGL {

    ShaderParameter::ShaderParameter(Program& program, std::string const& identifier)
    {
        if (glGetUniformLocation != nullptr)
            GLCHECK(this->_uniform = glGetUniformLocation(program.GetProgram(), identifier.c_str()));
        else if (glGetUniformLocationARB != nullptr)
            GLCHECK(this->_uniform = glGetUniformLocationARB(program.GetProgram(), identifier.c_str()));
        else
            throw std::runtime_error("glGetUniformLocation unsupported");
    }

    ShaderParameter::~ShaderParameter()
    {
    }

    void ShaderParameter::Set(bool value)
    {
        GLCHECK(glUniform1i(this->_uniform, value ? 1 : 0));
    }

    void ShaderParameter::Set(int value)
    {
        GLCHECK(glUniform1i(this->_uniform, value));
    }

    void ShaderParameter::Set(float value)
    {
        GLCHECK(glUniform1f(this->_uniform, value));
    }

    void ShaderParameter::Set(glm::detail::tvec2<float> const& vector)
    {
        GLCHECK(glUniform2fv(this->_uniform, 1, (float const*)&vector));
    }

    void ShaderParameter::Set(glm::detail::tvec3<float> const& vector)
    {
        GLCHECK(glUniform3fv(this->_uniform, 1, (float const*)&vector));
    }

    void ShaderParameter::Set(glm::detail::tvec4<float> const& vector)
    {
        GLCHECK(glUniform4fv(this->_uniform, 1, (float const*)&vector));
    }

    void ShaderParameter::Set(glm::detail::tmat4x4<float> const& matrix, bool)
    {
        //GLCHECK(glUniformMatrix4fv(this->_uniform, 1, false, (float const*)&matrix));
        auto tmp = glm::transpose(matrix);
        GLCHECK(glUniform4fv(this->_uniform, 4, (float const*)&tmp));
        //for (int i = 0; i < 4; ++i)
        //    GLCHECK(glUniform4fv(this->_uniform + i, 1, (float const*)&(matrix[i])));
    }

    void ShaderParameter::Set(std::vector<glm::mat4x4> const& matrices)
    {
        //GLCHECK(glUniformMatrix4fv(this->_uniform, (GLuint)matrices.size(), false, (float const*)matrices.data()));
        GLCHECK(glUniform4fv(this->_uniform, 4 * matrices.size(), (float const*)matrices.data()));
    }

    void ShaderParameter::Set(ITexture2D& texture)
    {
        Texture2D& tex = reinterpret_cast<Texture2D&>(texture);
        assert(tex.GetBindID() >= 0 && "Faut bind la texture avant.");
        GLCHECK(glUniform1i(this->_uniform, tex.GetBindID()));
    }

}}}
