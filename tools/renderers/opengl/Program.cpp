#include "tools/precompiled.hpp"

#include "tools/renderers/GLRenderer.hpp"
#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/Program.hpp"
#include "tools/renderers/opengl/ShaderParameter.hpp"
#include "tools/renderers/opengl/VertexBuffer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    Program::Program(GLRenderer& renderer, VertexProgram&& vertex, FragmentProgram&& fragment) :
        _renderer(renderer),
        _nbTextures(0),
        _vertexProgram(std::move(vertex)),
        _fragmentProgram(std::move(fragment))
    {
        for (auto& it: this->_attributes)
            it = -1;
        if (glLinkProgram != nullptr)
        {
            GLCHECK(this->_program = glCreateProgram());
            GLCHECK(glAttachShader(this->_program, this->_vertexProgram.GetShader()));
            GLCHECK(glAttachShader(this->_program, this->_fragmentProgram.GetShader()));
            GLCHECK(glLinkProgram(this->_program));
        }
        else if (glLinkProgramARB != nullptr)
        {
            GLCHECK(this->_program = glCreateProgramObjectARB());
            GLCHECK(glAttachObjectARB(this->_program, this->_vertexProgram.GetShader()));
            GLCHECK(glAttachObjectARB(this->_program, this->_fragmentProgram.GetShader()));
            GLCHECK(glLinkProgramARB(this->_program));
        }
        else
            throw std::runtime_error("glLinkProgram unsupported");

        for (int t = 0; t < (int)VertexAttributeUsage::Max; ++t)
        {
            auto& param = this->GetParameter(VertexAttributeUsage::typeToName[(VertexAttributeUsage::Type)t]);
            this->_shaderParameters[t] = param.IsUseable() ? &param : 0;
        }
    }

    Program::~Program()
    {
        if (glLinkProgram != nullptr)
            GLCHECK(glDeleteProgram(this->_program));
        else if (glLinkProgramARB != nullptr)
            GLCHECK(glDeleteProgramsARB(1, &this->_program));
        else
            throw std::runtime_error("glDeleteProgram unsupported");
    }

    void Program::SetAttributeUsage(std::string const& identifier, VertexAttributeUsage::Type usage)
    {
        if (glGetAttribLocation != nullptr)
            GLCHECK(this->_attributes[usage] = glGetAttribLocation(this->_program, identifier.c_str()));
        else if (glGetAttribLocationARB != nullptr)
            GLCHECK(this->_attributes[usage] = glGetAttribLocationARB(this->_program, identifier.c_str()));
        else
            throw std::runtime_error("glGetAttribLocation unsupported");
    }

    IShaderParameter& Program::GetParameter(std::string const& identifier)
    {
        auto it = this->_parameters.find(identifier);
        if (it == this->_parameters.end())
        {
            auto ptr = new ShaderParameter(*this, identifier);
            auto const& pair = this->_parameters.insert(std::make_pair(identifier, std::unique_ptr<IShaderParameter>(ptr)));
            return *pair.first->second;
        }
        return *it->second;
    }

    void Program::Begin()
    {
        this->_renderer.SetCurrentProgram(*this);
        GLCHECK(glUseProgram(this->_program));
    }

    void Program::End()
    {
    }

}}}
