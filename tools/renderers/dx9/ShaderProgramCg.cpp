#ifdef _WIN32

#include "tools/precompiled.hpp"

#include "tools/renderers/GLRenderer.hpp"
#include "tools/renderers/opengl/opengl.hpp"
#include "tools/renderers/opengl/ShaderProgramCg.hpp"
#include "tools/renderers/opengl/ShaderParameterCg.hpp"

namespace Tools { namespace Renderers { namespace OpenGL {

    ShaderProgramCg::ShaderProgramCg(GLRenderer& renderer, std::string const& effect) :
        _renderer(renderer),
        _ctx(_renderer.GetCgContext()),
        _nbTextures(0),
        _mvp(0),
        _mv(0),
        _model(0),
        _view(0),
        _projection(0),
        _pass(0)
    {
        this->_effect = cgCreateEffect(this->_ctx, effect.c_str(), 0);
        this->_technique = cgGetFirstTechnique(this->_effect);
        while (this->_technique && cgValidateTechnique(this->_technique) == CG_FALSE)
            this->_technique = cgGetNextTechnique(this->_technique);
        if (this->_technique == 0)
            throw std::runtime_error("Shaders: No valid technique");
        this->_mvp = cgGetEffectParameterBySemantic(this->_effect, "WorldViewProjection");
        this->_mv = cgGetEffectParameterBySemantic(this->_effect, "WorldView");
        this->_model = cgGetEffectParameterBySemantic(this->_effect, "World");
        this->_view = cgGetEffectParameterBySemantic(this->_effect, "View");
        this->_projection = cgGetEffectParameterBySemantic(this->_effect, "Projection");
    }

    ShaderProgramCg::~ShaderProgramCg()
    {
        if (this->_mvp)
            cgDestroyParameter(this->_mvp);
        if (this->_mv)
            cgDestroyParameter(this->_mv);
        if (this->_model)
            cgDestroyParameter(this->_model);
        if (this->_view)
            cgDestroyParameter(this->_view);
        if (this->_projection)
            cgDestroyParameter(this->_projection);
        cgDestroyEffect(this->_effect);
    }

    std::unique_ptr<IShaderParameter> ShaderProgramCg::GetParameter(std::string const& identifier)
    {
        return std::unique_ptr<IShaderParameter>(new ShaderParameterCg(*this, identifier));
    }

    std::unique_ptr<IShaderParameter> ShaderProgramCg::GetParameterFromSemantic(std::string const& semantic)
    {
        return std::unique_ptr<IShaderParameter>(new ShaderParameterCg(*this, cgGetEffectParameterBySemantic(this->_effect, semantic.c_str())));
    }

    void ShaderProgramCg::SetParameterUsage(std::string const& identifier, ShaderParameterUsage::Type usage)
    {
        switch (usage)
        {
        case ShaderParameterUsage::ModelViewProjectionMatrix:
            this->_mvp = cgGetNamedEffectParameter(this->_effect, identifier.c_str());
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
                cgSetMatrixParameterfc(this->_mvp, this->_renderer.GetModelViewProjectionMatrix().mm);
            break;

        case ShaderParameterUsage::ModelViewMatrix:
            if (this->_mv)
                cgSetMatrixParameterfc(this->_mv, (this->_renderer.GetModelMatrix() * this->_renderer.GetViewMatrix()).mm);
            break;

        case ShaderParameterUsage::ModelMatrix:
            if (this->_model)
                cgSetMatrixParameterfc(this->_model, this->_renderer.GetModelMatrix().mm);
            break;

        case ShaderParameterUsage::ViewMatrix:
            if (this->_view)
                cgSetMatrixParameterfc(this->_view, this->_renderer.GetViewMatrix().mm);
            break;

        case ShaderParameterUsage::ProjectionMatrix:
            if (this->_projection)
                cgSetMatrixParameterfc(this->_projection, this->_renderer.GetProjectionMatrix().mm);
            break;

        default:
            throw std::invalid_argument("ShaderProgramCg::UpdateParameter");
        }
    }

    void ShaderProgramCg::UpdateCurrentPass()
    {
        if (this->_pass != 0)
            cgUpdatePassParameters(this->_pass);
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

}}}

#endif
