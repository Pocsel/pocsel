#include <iostream>

#include "opengl.hpp"

#include "tools/renderers/GLRenderer.hpp"
#include "ShaderProgram.hpp"
#include "ShaderParameter.hpp"

using namespace Tools::Renderers;
using namespace Tools::Renderers::OpenGL;

ShaderProgram::ShaderProgram(GLRenderer& renderer, std::string const& vertexShader, std::string const& fragmentShader) :
    _renderer(renderer),
    _nbTextures(0),
    _mvpId(-1),
    _modelId(-1),
    _viewId(-1),
    _projectionId(-1)
{
    GLuint vShader, fShader;
    GLCHECK(vShader = glCreateShaderObjectARB(GL_VERTEX_SHADER));
    GLCHECK(fShader = glCreateShaderObjectARB(GL_FRAGMENT_SHADER));

    GLint vsLen = vertexShader.length();
    GLint fsLen = fragmentShader.length();

    auto temp = vertexShader.c_str();
    GLCHECK(glShaderSourceARB(vShader, 1, &temp, &vsLen));
    temp = fragmentShader.c_str();
    GLCHECK(glShaderSourceARB(fShader, 1, &temp, &fsLen));

    GLCHECK(glCompileShaderARB(vShader));
    GLCHECK(glCompileShaderARB(fShader));

    GLint compiled;
    GLCHECK(glGetObjectParameterivARB(vShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled));
    if (compiled == GL_FALSE)
    {
        char info[8192];
        GLsizei length;
        GLCHECK(glGetInfoLogARB(vShader, sizeof(info), &length, info));
        std::cerr << "Vertex Shader error: " << info << std::endl;
    }

    GLCHECK(glGetObjectParameterivARB(fShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled));
    if (compiled == GL_FALSE)
    {
        char info[8192];
        GLsizei length;
        GLCHECK(glGetInfoLogARB(fShader, sizeof(info), &length, info));
        std::cerr << "Fragment Shader error: " << info << std::endl;
    }

    GLCHECK(this->_id = glCreateProgramObjectARB());

    GLCHECK(glAttachObjectARB(this->_id, vShader));
    GLCHECK(glAttachObjectARB(this->_id, fShader));
    GLCHECK(glLinkProgramARB(this->_id));

    GLCHECK(glGetObjectParameterivARB(vShader, GL_OBJECT_COMPILE_STATUS_ARB, &compiled));
    if (compiled == GL_FALSE)
    {
        char info[8192];
        GLsizei length;
        GLCHECK(glGetInfoLogARB(this->_id, sizeof(info), &length, info));
    }

    GLCHECK(glDeleteObjectARB(vShader));
    GLCHECK(glDeleteObjectARB(fShader));
}

ShaderProgram::~ShaderProgram()
{
    GLCHECK(glDeleteProgramsARB(1, &this->_id));
}

std::unique_ptr<IShaderParameter> ShaderProgram::GetParameter(std::string const& identifier)
{
    return std::unique_ptr<IShaderParameter>(new ShaderParameter(*this, identifier));
}

void ShaderProgram::SetParameterUsage(std::string const& identifier, ShaderParameterUsage::Type usage)
{
    switch (usage)
    {
    case ShaderParameterUsage::ModelViewProjectionMatrix:
        GLCHECK(this->_mvpId = glGetUniformLocationARB(this->_id, identifier.c_str()));
        break;

    case ShaderParameterUsage::ModelMatrix:
        GLCHECK(this->_modelId = glGetUniformLocationARB(this->_id, identifier.c_str()));
        break;

    case ShaderParameterUsage::ViewMatrix:
        GLCHECK(this->_viewId = glGetUniformLocationARB(this->_id, identifier.c_str()));
        break;

    case ShaderParameterUsage::ProjectionMatrix:
        GLCHECK(this->_projectionId = glGetUniformLocationARB(this->_id, identifier.c_str()));
        break;

    default:
        throw std::invalid_argument("ShaderProgram::SetParameterUsage");
    }
}

void ShaderProgram::UpdateParameter(ShaderParameterUsage::Type usage)
{
    switch (usage)
    {
    case ShaderParameterUsage::ModelViewProjectionMatrix:
        if (this->_mvpId != -1)
            GLCHECK(glUniformMatrix4fvARB(this->_mvpId, 1, GL_FALSE, this->_renderer.GetModelViewProjectionMatrix().mm));
        this->_renderer.SetMatrixMode(GL_MODELVIEW);
        GLCHECK(glLoadMatrixf((this->_renderer.GetModelMatrix() * this->_renderer.GetViewMatrix()).mm));
        this->_renderer.SetMatrixMode(GL_PROJECTION);
        GLCHECK(glLoadMatrixf(this->_renderer.GetProjectionMatrix().mm));
        break;

    case ShaderParameterUsage::ModelMatrix:
        if (this->_modelId != -1)
            GLCHECK(glUniformMatrix4fvARB(this->_modelId, 1, GL_FALSE, this->_renderer.GetModelMatrix().mm));
        this->_renderer.SetMatrixMode(GL_MODELVIEW);
        GLCHECK(glLoadMatrixf((this->_renderer.GetModelMatrix() * this->_renderer.GetViewMatrix()).mm));
        break;

    case ShaderParameterUsage::ViewMatrix:
        if (this->_viewId != -1)
            GLCHECK(glUniformMatrix4fvARB(this->_viewId, 1, GL_FALSE, this->_renderer.GetViewMatrix().mm));
        this->_renderer.SetMatrixMode(GL_MODELVIEW);
        GLCHECK(glLoadMatrixf((this->_renderer.GetModelMatrix() * this->_renderer.GetViewMatrix()).mm));
        break;

    case ShaderParameterUsage::ProjectionMatrix:
        if (this->_projectionId != -1)
            GLCHECK(glUniformMatrix4fvARB(this->_projectionId, 1, GL_FALSE, this->_renderer.GetProjectionMatrix().mm));
        this->_renderer.SetMatrixMode(GL_PROJECTION);
        GLCHECK(glLoadMatrixf(this->_renderer.GetProjectionMatrix().mm));
        break;

    default:
        throw std::invalid_argument("ShaderProgram::UpdateParameter");
    }
}

void ShaderProgram::Activate()
{
    this->_renderer.SetCurrentProgram(*this);
    GLCHECK(glUseProgramObjectARB(this->_id));
    this->_nbTextures = 0;

    if (this->_projectionId != -1)
        this->UpdateParameter(ShaderParameterUsage::ProjectionMatrix);
    if (this->_modelId != -1)
        this->UpdateParameter(ShaderParameterUsage::ModelMatrix);
    if (this->_viewId != -1)
        this->UpdateParameter(ShaderParameterUsage::ViewMatrix);
    this->UpdateParameter(ShaderParameterUsage::ModelViewProjectionMatrix);
}
