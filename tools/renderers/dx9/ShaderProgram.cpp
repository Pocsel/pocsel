#ifdef _WIN32

#include "tools/precompiled.hpp"
#include "tools/logger/Logger.hpp"

#include "tools/renderers/DX9Renderer.hpp"
#include "tools/renderers/dx9/directx.hpp"
#include "tools/renderers/dx9/ShaderProgram.hpp"
#include "tools/renderers/dx9/ShaderParameter.hpp"

namespace Tools { namespace Renderers { namespace DX9 {

    ShaderProgram::ShaderProgram(DX9Renderer& renderer, std::string const& effect) :
        _renderer(renderer),
        _nbTextures(0),
        _mvp(0),
        _mv(0),
        _model(0),
        _view(0),
        _projection(0),
        _pass(0)
    {
        static D3DXMACRO defines[] = {
                {"DIRECTX", "1"},
                0
            };
        LPD3DXBUFFER errors;
        D3DXCreateEffect(this->_renderer.GetDevice(), effect.c_str(), (UINT)effect.length(), defines, 0, 0, 0, &this->_effect, &errors);
        if (errors)
        {
            Tools::debug << "DX9::Shader errors: " << (char const*)errors->GetBufferPointer() << std::endl;
            throw std::runtime_error(std::string("DX9::Shader: ") + (char const*)errors->GetBufferPointer());
        }

        DXCHECKERROR(this->_effect->FindNextValidTechnique(0, &this->_technique));
        
        this->_mvp = this->_effect->GetParameterBySemantic(0, "WorldViewProjection");
        this->_mv = this->_effect->GetParameterBySemantic(0, "WorldView");
        this->_model = this->_effect->GetParameterBySemantic(0, "World");
        this->_view = this->_effect->GetParameterBySemantic(0, "View");
        this->_projection = this->_effect->GetParameterBySemantic(0, "Projection");
    }

    ShaderProgram::~ShaderProgram()
    {
        this->_effect->Release();
    }

    std::unique_ptr<IShaderParameter> ShaderProgram::GetParameter(std::string const& identifier)
    {
        return std::unique_ptr<IShaderParameter>(new ShaderParameter(*this, identifier));
    }

    std::unique_ptr<IShaderParameter> ShaderProgram::GetParameterFromSemantic(std::string const& semantic)
    {
        return std::unique_ptr<IShaderParameter>(new ShaderParameter(*this, this->_effect->GetParameterBySemantic(0, semantic.c_str())));
    }

    void ShaderProgram::SetParameterUsage(std::string const& identifier, ShaderParameterUsage::Type usage)
    {
        switch (usage)
        {
        case ShaderParameterUsage::ModelViewProjectionMatrix:
            this->_mvp = this->_effect->GetParameterByName(0, identifier.c_str());
            break;

        case ShaderParameterUsage::ModelViewMatrix:
            this->_mv = this->_effect->GetParameterByName(0, identifier.c_str());
            break;

        case ShaderParameterUsage::ModelMatrix:
            this->_model = this->_effect->GetParameterByName(0, identifier.c_str());
            break;

        case ShaderParameterUsage::ViewMatrix:
            this->_view = this->_effect->GetParameterByName(0, identifier.c_str());
            break;

        case ShaderParameterUsage::ProjectionMatrix:
            this->_projection = this->_effect->GetParameterByName(0, identifier.c_str());
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
            if (this->_mvp)
                this->_effect->SetMatrixTranspose(this->_mvp, (D3DXMATRIX const*)this->_renderer.GetModelViewProjectionMatrix().mm);
            break;

        case ShaderParameterUsage::ModelViewMatrix:
            if (this->_mv)
                this->_effect->SetMatrixTranspose(this->_mv, (D3DXMATRIX const*)(this->_renderer.GetModelMatrix() * this->_renderer.GetViewMatrix()).mm);
            break;

        case ShaderParameterUsage::ModelMatrix:
            if (this->_model)
                this->_effect->SetMatrixTranspose(this->_model, (D3DXMATRIX const*)this->_renderer.GetModelMatrix().mm);
            break;

        case ShaderParameterUsage::ViewMatrix:
            if (this->_view)
                this->_effect->SetMatrixTranspose(this->_view, (D3DXMATRIX const*)this->_renderer.GetViewMatrix().mm);
            break;

        case ShaderParameterUsage::ProjectionMatrix:
            if (this->_projection)
                this->_effect->SetMatrixTranspose(this->_projection, (D3DXMATRIX const*)this->_renderer.GetProjectionMatrix().mm);
            break;

        default:
            throw std::invalid_argument("ShaderProgram::UpdateParameter");
        }
    }

    void ShaderProgram::UpdateCurrentPass()
    {
        DXCHECKERROR(this->_effect->CommitChanges());
    }

    void ShaderProgram::BeginPass()
    {
        this->_renderer.SetCurrentProgram(*this);
        if (this->_pass == 0)
            DXCHECKERROR(this->_effect->Begin(&this->_passCount, 0));
        DXCHECKERROR(this->_effect->BeginPass(this->_pass));
        this->_nbTextures = 0;
    }

    bool ShaderProgram::EndPass()
    {
        DXCHECKERROR(this->_effect->EndPass());
        this->_pass = (this->_pass + 1) % this->_passCount;
        if (this->_pass == 0)
            DXCHECKERROR(this->_effect->End());
        return this->_pass != 0;
    }

}}}

#endif