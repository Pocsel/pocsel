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
        _vp(0),
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
        D3DXCreateEffect(this->_renderer.GetDevice(), effect.c_str(), (UINT)effect.length(), defines, 0, 0, this->_renderer.GetEffectPool(), &this->_effect, &errors);
        if (errors)
        {
            Tools::debug << "DX9::Shader errors: " << (char const*)errors->GetBufferPointer() << std::endl;
            throw std::runtime_error(std::string("DX9::Shader: ") + (char const*)errors->GetBufferPointer());
        }

        DXCHECKERROR(this->_effect->FindNextValidTechnique(0, &this->_technique));
        
        this->_mvp = this->_effect->GetParameterBySemantic(0, "WorldViewProjection");
        this->_vp = this->_effect->GetParameterBySemantic(0, "ViewProjection");
        this->_mv = this->_effect->GetParameterBySemantic(0, "WorldView");
        this->_model = this->_effect->GetParameterBySemantic(0, "World");
        this->_view = this->_effect->GetParameterBySemantic(0, "View");
        this->_projection = this->_effect->GetParameterBySemantic(0, "Projection");
        this->_modelInverse = this->_effect->GetParameterBySemantic(0, "WorldInverse");
        this->_viewInverse = this->_effect->GetParameterBySemantic(0, "ViewInverse");
        this->_projectionInverse = this->_effect->GetParameterBySemantic(0, "ProjectionInverse");
        this->_mvpInverse = this->_effect->GetParameterBySemantic(0, "WorldViewProjectionInverse");
        this->_viewProjectionInverse = this->_effect->GetParameterBySemantic(0, "ViewProjectionInverse");
        this->_worldViewInverseTranspose = this->_effect->GetParameterByName(0, "worldViewInverseTranspose");
    }

    ShaderProgram::~ShaderProgram()
    {
        this->_renderer.Unregister(*this);
        this->_effect->Release();
    }

    std::unique_ptr<IShaderParameter> ShaderProgram::GetParameter(std::string const& identifier)
    {
        auto param = new ShaderParameter(*this, identifier);
        if (param->IsUseable())
            return std::unique_ptr<IShaderParameter>(param);
        Tools::Delete(param);
        return std::unique_ptr<IShaderParameter>();
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

        case ShaderParameterUsage::ViewProjectionMatrix:
            this->_vp = this->_effect->GetParameterByName(0, identifier.c_str());
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
                this->_effect->SetMatrixTranspose(this->_mvp, (D3DXMATRIX const*)&this->_renderer.GetModelViewProjectionMatrix()[0]);
            if (this->_mvpInverse)
            {
                auto tmp = glm::inverse(this->_renderer.GetProjectionMatrix());
                this->_effect->SetMatrixTranspose(this->_mvpInverse, (D3DXMATRIX const*)&tmp[0]);
            }
            break;

        case ShaderParameterUsage::ViewProjectionMatrix:
            if (this->_vp)
            {
                auto tmp = this->_renderer.GetProjectionMatrix() * this->_renderer.GetViewMatrix();
                this->_effect->SetMatrixTranspose(this->_vp, (D3DXMATRIX const*)&tmp[0]);
            }
            if (this->_viewProjectionInverse)
            {
                auto tmp = glm::inverse(this->_renderer.GetProjectionMatrix() * this->_renderer.GetViewMatrix());
                this->_effect->SetMatrixTranspose(this->_viewProjectionInverse, (D3DXMATRIX const*)&tmp[0]);
            }
            if (this->_worldViewInverseTranspose)
            {
                auto tmp = glm::inverse(this->_renderer.GetViewMatrix() * this->_renderer.GetModelMatrix());
                this->_effect->SetMatrix(this->_worldViewInverseTranspose, (D3DXMATRIX const*)&tmp[0]);
            }
            break;

        case ShaderParameterUsage::ModelViewMatrix:
            if (this->_mv)
            {
                auto tmp = this->_renderer.GetViewMatrix() * this->_renderer.GetModelMatrix();
                this->_effect->SetMatrixTranspose(this->_mv, (D3DXMATRIX const*)&tmp[0]);
            }
            break;

        case ShaderParameterUsage::ModelMatrix:
            if (this->_model)
                this->_effect->SetMatrixTranspose(this->_model, (D3DXMATRIX const*)&this->_renderer.GetModelMatrix()[0]);
            if (this->_modelInverse)
            {
                auto tmp = glm::inverse(this->_renderer.GetModelMatrix());
                this->_effect->SetMatrixTranspose(this->_modelInverse, (D3DXMATRIX const*)&tmp[0]);
            }
            break;

        case ShaderParameterUsage::ViewMatrix:
            if (this->_view)
                this->_effect->SetMatrixTranspose(this->_view, (D3DXMATRIX const*)&this->_renderer.GetViewMatrix()[0]);
            if (this->_viewInverse)
            {
                auto tmp = glm::inverse(this->_renderer.GetViewMatrix());
                this->_effect->SetMatrixTranspose(this->_viewInverse, (D3DXMATRIX const*)&tmp[0]);
            }
            break;

        case ShaderParameterUsage::ProjectionMatrix:
            if (this->_projection)
                this->_effect->SetMatrixTranspose(this->_projection, (D3DXMATRIX const*)&this->_renderer.GetProjectionMatrix()[0]);
            if (this->_projectionInverse)
            {
                auto tmp = glm::inverse(this->_renderer.GetProjectionMatrix());
                this->_effect->SetMatrixTranspose(this->_projectionInverse, (D3DXMATRIX const*)&tmp[0]);
            }
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
