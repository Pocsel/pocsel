#include <stdexcept>
#include <iostream>

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/GLRenderer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    void GLCheckError(std::string const& file, unsigned int line, char const* function)
    {
        // Get the last error
        GLenum errorCode = glGetError();

        if (errorCode == GL_NO_ERROR)
            return;

        std::string error = "unknown error";
        std::string desc  = "no description";

        // Decode the error code
        switch (errorCode)
        {
        case GL_INVALID_ENUM:
        {
            error = "GL_INVALID_ENUM";
            desc  = "an unacceptable value has been specified for an enumerated argument";
            break;
        }

        case GL_INVALID_VALUE:
        {
            error = "GL_INVALID_VALUE";
            desc  = "a numeric argument is out of range";
            break;
        }

        case GL_INVALID_OPERATION:
        {
            error = "GL_INVALID_OPERATION";
            desc  = "the specified operation is not allowed in the current state";
            break;
        }

        case GL_STACK_OVERFLOW:
        {
            error = "GL_STACK_OVERFLOW";
            desc  = "this command would cause a stack overflow";
            break;
        }

        case GL_STACK_UNDERFLOW:
        {
            error = "GL_STACK_UNDERFLOW";
            desc  = "this command would cause a stack underflow";
            break;
        }

        case GL_OUT_OF_MEMORY:
        {
            error = "GL_OUT_OF_MEMORY";
            desc  = "there is not enough memory left to execute the command";
            break;
        }

        case GL_INVALID_FRAMEBUFFER_OPERATION_EXT:
        {
            error = "GL_INVALID_FRAMEBUFFER_OPERATION_EXT";
            desc  = "the object bound to FRAMEBUFFER_BINDING_EXT is not \"framebuffer complete\"";
            break;
        }
        }

        throw std::runtime_error(
                    std::string("An internal OpenGL call failed in ")
                    + file.substr(file.find_last_of("\\/") + 1) + " (" + ToString(line) + ", " + function + ") : "
                    + error + " (code: " + ToString(errorCode) + ")" + ", " + desc
                    + "\n");
    }

}}}
