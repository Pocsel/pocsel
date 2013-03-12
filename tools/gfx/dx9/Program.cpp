#ifdef _WIN32

#include "tools/precompiled.hpp"
#include "tools/logger/Logger.hpp"

#include "tools/gfx/DX9Renderer.hpp"
#include "tools/gfx/dx9/directx.hpp"
#include "tools/gfx/dx9/Program.hpp"
#include "tools/gfx/dx9/ShaderParameter.hpp"

namespace Tools { namespace Gfx { namespace DX9 {

    Program::Program(DX9Renderer& renderer, VertexProgram&& vertex, FragmentProgram&& fragment) :
        _renderer(renderer),
        _vertexProgram(std::move(vertex)),
        _fragmentProgram(std::move(fragment))
    {

    }

    Program::~Program()
    {
        this->_renderer.Unregister(*this);
    }

    void Program::SetAttributeUsage(std::string const& identifier, VertexAttributeUsage::Type usage)
    {
        
    }

    void Program::SetParameterUsage(ShaderParameterUsage::Type usage, std::string const& identifier)
    {
        auto& param = this->GetParameter(identifier);
        this->_defaultParameters[usage] = &param;
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

    void Program::UpdateParameter(ShaderParameterUsage::Type usage)
    {
        auto const& it = this->_defaultParameters.find(usage);
        if (it == this->_defaultParameters.end())
            return;
        switch (usage)
        {
        case ShaderParameterUsage::ModelMatrix: it->second->Set(this->_renderer.GetModelMatrix()); break;
        case ShaderParameterUsage::ModelViewMatrix: it->second->Set(this->_renderer.GetViewMatrix() * this->_renderer.GetModelMatrix()); break;
        case ShaderParameterUsage::ModelViewProjectionMatrix: it->second->Set(this->_renderer.GetModelViewProjectionMatrix()); break;
        case ShaderParameterUsage::ProjectionMatrix: it->second->Set(this->_renderer.GetProjectionMatrix()); break;
        case ShaderParameterUsage::ViewMatrix: it->second->Set(this->_renderer.GetViewMatrix()); break;
        case ShaderParameterUsage::ViewProjectionMatrix: it->second->Set(this->_renderer.GetProjectionMatrix() * this->_renderer.GetViewMatrix()); break;
        }
    }

    void Program::Update()
    {
    }

    void Program::Begin()
    {
        this->_renderer.SetCurrentProgram(*this);
        this->_renderer.GetDevice()->SetVertexShader(this->GetVertexProgram().GetShader());
        this->_renderer.GetDevice()->SetPixelShader(this->GetFragmentProgram().GetShader());
    }

    void Program::End()
    {
    }

}}}

#endif
