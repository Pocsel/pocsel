#include "tools/renderers/GLRenderer.hpp"

#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/ShaderProgramNull.hpp"
#include "tools/renderers/opengl/ShaderParameterNull.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    ShaderProgramNull::ShaderProgramNull(GLRenderer& renderer)
        : _renderer(renderer)
    {
    }

    std::unique_ptr<IShaderParameter> ShaderProgramNull::GetParameter(std::string const&)
    {
        return std::unique_ptr<IShaderParameter>(new ShaderParameterNull());
    }

    std::unique_ptr<IShaderParameter> ShaderProgramNull::GetParameterFromSemantic(std::string const& semantic)
    {
        return std::unique_ptr<IShaderParameter>(new ShaderParameterNull());
    }

    void ShaderProgramNull::UpdateParameter(ShaderParameterUsage::Type usage)
    {
        switch (usage)
        {
        case ShaderParameterUsage::ModelViewProjectionMatrix:
            {
                this->_renderer.SetMatrixMode(GL_MODELVIEW);
                auto mat = this->_renderer.GetViewMatrix() * this->_renderer.GetModelMatrix();
                GLCHECK(glLoadMatrixf((float*)&mat));
                this->_renderer.SetMatrixMode(GL_PROJECTION);
                GLCHECK(glLoadMatrixf((float*)&this->_renderer.GetProjectionMatrix()));
            }
            break;

        case ShaderParameterUsage::ModelViewMatrix:
        case ShaderParameterUsage::ModelMatrix:
        case ShaderParameterUsage::ViewMatrix:
            {
                this->_renderer.SetMatrixMode(GL_MODELVIEW);
                auto mat = this->_renderer.GetViewMatrix() * this->_renderer.GetModelMatrix();
                GLCHECK(glLoadMatrixf((float*)&mat));
            }
            break;

        case ShaderParameterUsage::ProjectionMatrix:
            this->_renderer.SetMatrixMode(GL_PROJECTION);
            GLCHECK(glLoadMatrixf((float*)&this->_renderer.GetProjectionMatrix()));
            break;

        default:
            throw std::invalid_argument("ShaderProgram::UpdateParameter");
        }
    }

    void ShaderProgramNull::BeginPass()
    {
        this->_renderer.SetCurrentProgram(*this);

        this->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
    }

    bool ShaderProgramNull::EndPass()
    {
        return false;
    }

}}}
