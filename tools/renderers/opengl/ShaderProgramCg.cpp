#include "tools/precompiled.hpp"

#include "tools/renderers/GLRenderer.hpp"
#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/ShaderProgramCg.hpp"
#include "tools/renderers/opengl/ShaderParameterCg.hpp"
#include "tools/renderers/opengl/VertexBuffer.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    ShaderProgramCg::ShaderProgramCg(GLRenderer& renderer, std::string const& effect) :
        _renderer(renderer),
        _ctx(_renderer.GetCgContext()),
        _nbTextures(0),
        _mvp(0),
        _vp(0),
        _mv(0),
        _model(0),
        _view(0),
        _projection(0),
        _modelInverse(0),
        _viewInverse(0),
        _projectionInverse(0),
        _pass(0)
    {
        this->_effect = cgCreateEffect(this->_ctx, effect.c_str(), 0);
        this->_technique = cgGetFirstTechnique(this->_effect);
        while (this->_technique && cgValidateTechnique(this->_technique) == CG_FALSE)
            this->_technique = cgGetNextTechnique(this->_technique);
        if (this->_technique == 0)
            throw std::runtime_error("Shaders: No valid technique");
        this->_mvp = cgGetEffectParameterBySemantic(this->_effect, "WorldViewProjection");
        this->_vp = cgGetEffectParameterBySemantic(this->_effect, "ViewProjection");
        this->_mv = cgGetEffectParameterBySemantic(this->_effect, "WorldView");
        this->_model = cgGetEffectParameterBySemantic(this->_effect, "World");
        this->_view = cgGetEffectParameterBySemantic(this->_effect, "View");
        this->_projection = cgGetEffectParameterBySemantic(this->_effect, "Projection");
        this->_modelInverse = cgGetEffectParameterBySemantic(this->_effect, "WorldInverse");
        this->_viewInverse = cgGetEffectParameterBySemantic(this->_effect, "ViewInverse");
        this->_projectionInverse = cgGetEffectParameterBySemantic(this->_effect, "ProjectionInverse");
        this->_viewProjectionInverse = cgGetEffectParameterBySemantic(this->_effect, "ViewProjectionInverse");
        this->_mvpInverse = cgGetEffectParameterBySemantic(this->_effect, "WorldViewProjectionInverse");
        this->_worldViewInverseTranspose = cgGetNamedEffectParameter(this->_effect, "worldViewInverseTranspose");
        for (int t = 0; t < (int)VertexAttributeUsage::Max; ++t)
        {
            auto& param = this->GetParameter(VertexAttributeUsage::typeToName[(VertexAttributeUsage::Type)t]);
            this->_shaderParameters[t] = param.IsUseable() ? &param : 0;
        }

        //Tools::debug << "*** Vertex Shader:\n" << cgGetProgramString(cgGetPassProgram(cgGetFirstPass(this->_technique), CG_VERTEX_DOMAIN), CG_COMPILED_PROGRAM);
        //Tools::debug << "*** Pixel  Shader:\n" << cgGetProgramString(cgGetPassProgram(cgGetFirstPass(this->_technique), CG_FRAGMENT_DOMAIN), CG_COMPILED_PROGRAM);
    }

    ShaderProgramCg::~ShaderProgramCg()
    {
        this->_parameters.clear();
        if (this->_mvp)
            cgDestroyParameter(this->_mvp);
        if (this->_vp)
            cgDestroyParameter(this->_vp);
        if (this->_mv)
            cgDestroyParameter(this->_mv);
        if (this->_model)
            cgDestroyParameter(this->_model);
        if (this->_view)
            cgDestroyParameter(this->_view);
        if (this->_projection)
            cgDestroyParameter(this->_projection);
        if (this->_modelInverse)
            cgDestroyParameter(this->_modelInverse);
        if (this->_viewInverse)
            cgDestroyParameter(this->_viewInverse);
        if (this->_projectionInverse)
            cgDestroyParameter(this->_projectionInverse);
        if (this->_viewProjectionInverse)
            cgDestroyParameter(this->_viewProjectionInverse);
        if (this->_mvpInverse)
            cgDestroyParameter(this->_mvpInverse);
        if (this->_worldViewInverseTranspose)
            cgDestroyParameter(this->_worldViewInverseTranspose);
        cgDestroyEffect(this->_effect);
    }

    IShaderParameter& ShaderProgramCg::GetParameter(std::string const& identifier)
    {
        auto it = this->_parameters.find(identifier);
        if (it == this->_parameters.end())
        {
            auto ptr = new ShaderParameterCg(*this, identifier);
            auto const& pair = this->_parameters.insert(std::make_pair(identifier, std::unique_ptr<IShaderParameter>(ptr)));
            return *pair.first->second;
        }
        return *it->second;
    }

    IShaderParameter& ShaderProgramCg::GetParameterFromSemantic(std::string const& semantic)
    {
        auto it = this->_parameters.find(semantic + "__semantic__");
        if (it == this->_parameters.end())
        {
            auto ptr = new ShaderParameterCg(cgGetEffectParameterBySemantic(this->_effect, semantic.c_str()));
            auto const& pair = this->_parameters.insert(std::make_pair(semantic + "__semantic__", std::unique_ptr<IShaderParameter>(ptr)));
            return *pair.first->second;
        }
        return *it->second;
    }

    void ShaderProgramCg::SetParameterUsage(std::string const& identifier, ShaderParameterUsage::Type usage)
    {
        switch (usage)
        {
        case ShaderParameterUsage::ModelViewProjectionMatrix:
            this->_mvp = cgGetNamedEffectParameter(this->_effect, identifier.c_str());
            break;

        case ShaderParameterUsage::ViewProjectionMatrix:
            this->_vp = cgGetNamedEffectParameter(this->_effect, identifier.c_str());
            break;

        case ShaderParameterUsage::ModelViewMatrix:
            this->_mv = cgGetNamedEffectParameter(this->_effect, identifier.c_str());
            break;

        case ShaderParameterUsage::ModelMatrix:
            this->_model = cgGetNamedEffectParameter(this->_effect, identifier.c_str());
            break;

        case ShaderParameterUsage::ViewMatrix:
            this->_view = cgGetNamedEffectParameter(this->_effect, identifier.c_str());
            break;

        case ShaderParameterUsage::ProjectionMatrix:
            this->_projection = cgGetNamedEffectParameter(this->_effect, identifier.c_str());
            break;

        default:
            throw std::invalid_argument("ShaderProgramCg::SetParameterUsage");
        }
    }

    void ShaderProgramCg::UpdateParameter(ShaderParameterUsage::Type usage)
    {
        switch (usage)
        {
        case ShaderParameterUsage::ModelViewProjectionMatrix:
            if (this->_mvp)
                cgSetMatrixParameterfc(this->_mvp, (float*)&this->_renderer.GetModelViewProjectionMatrix());
            if (this->_mvpInverse)
            {
                auto tmp = glm::inverse(this->_renderer.GetModelViewProjectionMatrix());
                cgSetMatrixParameterfc(this->_mvpInverse, (float*)&tmp);
            }
            break;

        case ShaderParameterUsage::ViewProjectionMatrix:
            if (this->_vp)
            {
                auto mat = this->_renderer.GetProjectionMatrix() * this->_renderer.GetViewMatrix();
                cgSetMatrixParameterfc(this->_vp, (float*)&mat);
            }
            if (this->_viewProjectionInverse)
            {
                auto mat = glm::inverse(this->_renderer.GetProjectionMatrix() * this->_renderer.GetViewMatrix());
                cgSetMatrixParameterfc(this->_viewProjectionInverse, (float*)&mat);
            }
            if (this->_worldViewInverseTranspose)
            {
                auto mat = glm::transpose(glm::inverse(this->_renderer.GetViewMatrix() * this->_renderer.GetModelMatrix()));
                cgSetMatrixParameterfc(this->_worldViewInverseTranspose, (float*)&mat);
            }
            break;

        case ShaderParameterUsage::ModelViewMatrix:
            if (this->_mv)
            {
                auto mat = this->_renderer.GetViewMatrix() * this->_renderer.GetModelMatrix();
                cgSetMatrixParameterfc(this->_mv, (float*)&mat);
            }
            break;

        case ShaderParameterUsage::ModelMatrix:
            if (this->_model)
                cgSetMatrixParameterfc(this->_model, (float*)&this->_renderer.GetModelMatrix());
            if (this->_modelInverse)
            {
                auto tmp = glm::inverse(this->_renderer.GetModelMatrix());
                cgSetMatrixParameterfc(this->_modelInverse, (float*)&tmp);
            }
            break;

        case ShaderParameterUsage::ViewMatrix:
            if (this->_view)
                cgSetMatrixParameterfc(this->_view, (float*)&this->_renderer.GetViewMatrix());
            if (this->_viewInverse)
            {
                auto tmp = glm::inverse(this->_renderer.GetViewMatrix());
                cgSetMatrixParameterfc(this->_viewInverse, (float*)&tmp);
            }
            break;

        case ShaderParameterUsage::ProjectionMatrix:
            if (this->_projection)
                cgSetMatrixParameterfc(this->_projection, (float*)&this->_renderer.GetProjectionMatrix());
            if (this->_projectionInverse)
            {
                auto tmp = glm::inverse(this->_renderer.GetProjectionMatrix());
                cgSetMatrixParameterfc(this->_projectionInverse, (float*)&tmp);
            }
            break;

        default:
            throw std::invalid_argument("ShaderProgramCg::UpdateParameter");
        }
    }

    void ShaderProgramCg::UpdateCurrentPass()
    {
        if (this->_pass != 0)
            cgUpdatePassParameters(this->_pass);
        this->UpdateParameters();
    }

    void ShaderProgramCg::BeginPass()
    {
        this->_renderer.SetCurrentProgram(*this);
        if (this->_pass == 0)
            this->_pass = cgGetFirstPass(this->_technique);
        cgSetPassState(this->_pass);
        this->_nbTextures = 0;
    }

    bool ShaderProgramCg::EndPass()
    {
        cgResetPassState(this->_pass);
        this->_pass = cgGetNextPass(this->_pass);
        return this->_pass != 0;
    }

    void ShaderProgramCg::UpdateParameters()
    {
        assert(this->_renderer.bindedVertexBuffer != 0 && "Il faut binder un vertex buffer");
        auto attrib = this->_renderer.bindedVertexBuffer->GetAttributes();
        auto cnt = this->_renderer.bindedVertexBuffer->GetAttributesCount();
        for (int i = 0; i < cnt; ++i)
        {
            //TODO attrib[i].location
        }
    }

}}}
