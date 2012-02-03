#include <iostream>

#include "tools/renderers/GLRenderer.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/ShaderProgramNull.hpp"
#include "tools/renderers/opengl/ShaderParameterNull.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    ShaderProgramNull::ShaderProgramNull(GLRenderer& renderer)
        : _renderer(renderer)
    {
    }

    std::unique_ptr<IShaderParameter> ShaderProgramNull::GetParameter(std::string const& identifier)
    {
        return std::unique_ptr<IShaderParameter>(new ShaderParameterNull());
    }

    void ShaderProgramNull::UpdateParameter(ShaderParameterUsage::Type usage)
    {
        switch (usage)
        {
        case ShaderParameterUsage::ModelViewProjectionMatrix:
            this->_renderer.SetMatrixMode(GL_MODELVIEW);
            GLCHECK(glLoadMatrixf((this->_renderer.GetModelMatrix() * this->_renderer.GetViewMatrix()).mm));
            this->_renderer.SetMatrixMode(GL_PROJECTION);
            GLCHECK(glLoadMatrixf(this->_renderer.GetProjectionMatrix().mm));
            break;

        case ShaderParameterUsage::ModelMatrix:
            this->_renderer.SetMatrixMode(GL_MODELVIEW);
            GLCHECK(glLoadMatrixf((this->_renderer.GetModelMatrix() * this->_renderer.GetViewMatrix()).mm));
            break;

        case ShaderParameterUsage::ViewMatrix:
            this->_renderer.SetMatrixMode(GL_MODELVIEW);
            GLCHECK(glLoadMatrixf((this->_renderer.GetModelMatrix() * this->_renderer.GetViewMatrix()).mm));
            break;

        case ShaderParameterUsage::ProjectionMatrix:
            this->_renderer.SetMatrixMode(GL_PROJECTION);
            GLCHECK(glLoadMatrixf(this->_renderer.GetProjectionMatrix().mm));
            break;

        default:
            throw std::invalid_argument("ShaderProgram::UpdateParameter");
        }
    }

    void ShaderProgramNull::Activate()
    {
        this->_renderer.SetCurrentProgram(*this);

        this->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
    }

}}}
