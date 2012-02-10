#include "tools/Color.hpp"
#include "tools/IRenderer.hpp"
#include "tools/Matrix4.hpp"
#include "tools/Vector2.hpp"
#include "tools/Vector3.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/Texture2D.hpp"
#include "tools/renderers/opengl/ShaderProgram.hpp"
#include "tools/renderers/opengl/ShaderParameter.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    ShaderParameter::ShaderParameter(ShaderProgram& program, std::string const& identifier) :
              _program(program), _identifier(identifier)
    {
        GLCHECK(this->_id = glGetUniformLocationARB(program.GetID(), identifier.c_str()));
    }

    void ShaderParameter::Set(bool value)
    {
        GLCHECK(glUniform1iARB(this->_id, value == 0 ? 0 : 1));
    }

    void ShaderParameter::Set(int value)
    {
        GLCHECK(glUniform1iARB(this->_id, value));
    }

    void ShaderParameter::Set(float value)
    {
        GLCHECK(glUniform1fARB(this->_id, value));
    }

    void ShaderParameter::Set(Color4<float> const& color)
    {
        GLCHECK(glUniform4fvARB(this->_id, 1, color.colors));
    }

    void ShaderParameter::Set(Vector2<float> const& vector)
    {
        GLCHECK(glUniform2fvARB(this->_id, 1, vector.coords));
    }

    void ShaderParameter::Set(Vector3<float> const& vector)
    {
        GLCHECK(glUniform3fvARB(this->_id, 1, vector.coords));
    }

    void ShaderParameter::Set(Matrix4<float> const& matrix)
    {
        GLCHECK(glUniformMatrix4fvARB(this->_id, 1, GL_FALSE, matrix.mm));
    }

    void ShaderParameter::Set(ITexture2D& texture)
    {
        Texture2D& tex = reinterpret_cast<Texture2D&>(texture);
        assert(tex.GetBindID() >= 0 && "Faut bind la texture avant.");
        GLCHECK(glUniform1iARB(this->_id, tex.GetBindID()));
    }

}}}
