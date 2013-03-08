#include "tools/precompiled.hpp"

#include "tools/IRenderer.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/Texture2D.hpp"
#include "tools/renderers/opengl/Program.hpp"
#include "tools/renderers/opengl/ShaderParameter.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

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
        glUniform1i(this->_uniform, value ? 1 : 0);
    }

    void ShaderParameter::Set(int value)
    {
        glUniform1i(this->_uniform, value);
    }

    void ShaderParameter::Set(float value)
    {
        glUniform1f(this->_uniform, value);
    }

    void ShaderParameter::Set(glm::detail::tvec2<float> const& vector)
    {
        glUniform2fv(this->_uniform, 2, (float const*)&vector);
    }

    void ShaderParameter::Set(glm::detail::tvec3<float> const& vector)
    {
        glUniform3fv(this->_uniform, 3, (float const*)&vector);
    }

    void ShaderParameter::Set(glm::detail::tvec4<float> const& vector)
    {
        glUniform4fv(this->_uniform, 4, (float const*)&vector);
    }

    void ShaderParameter::Set(glm::detail::tmat4x4<float> const& matrix, bool)
    {
        glUniformMatrix4fv(this->_uniform, 1, false, (float const*)&matrix);
    }

    void ShaderParameter::Set(std::vector<glm::mat4x4> const& matrices)
    {
        glUniformMatrix4fv(this->_uniform, (GLuint)matrices.size(), false, (float const*)matrices.data());
    }

    void ShaderParameter::Set(ITexture2D& texture)
    {
        Texture2D& tex = reinterpret_cast<Texture2D&>(texture);
        assert(tex.GetBindID() >= 0 && "Faut bind la texture avant.");
        glUniform1i(this->_uniform, tex.GetBindID());
    }

}}}
