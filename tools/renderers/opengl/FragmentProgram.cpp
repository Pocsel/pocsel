#include "tools/precompiled.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/FragmentProgram.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    FragmentProgram::FragmentProgram(std::string const& code)
    {
        char const* src[] = { code.c_str() };
        GLint len = code.length();
        if (glCreateShader != nullptr)
        {
            GLCHECK(this->_shader = glCreateShader(GL_FRAGMENT_SHADER));
            GLCHECK(glShaderSource(this->_shader, 1, src, &len));
            GLCHECK(glCompileShader(this->_shader));
        }
        else if (glCreateShaderObjectARB != nullptr)
        {
            GLCHECK(this->_shader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER_ARB));
            GLCHECK(glShaderSourceARB(this->_shader, 1, src, &len));
            GLCHECK(glCompileShaderARB(this->_shader));
        }
        else
            throw std::runtime_error("glCreateShader missing");
    }

    FragmentProgram::~FragmentProgram()
    {
        if (this->_shader == 0)
            return;
        if (glDeleteShader != nullptr)
            GLCHECK(glDeleteShader(this->_shader));
        else if (glDeleteObjectARB != nullptr)
            GLCHECK(glDeleteObjectARB(this->_shader));
        else
            throw std::runtime_error("glDeleteShader missing");
    }

}}}
