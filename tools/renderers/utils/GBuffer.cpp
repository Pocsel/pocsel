#include "tools/precompiled.hpp"
#include "tools/renderers/utils/GBuffer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    GBuffer::GBuffer(IRenderer& renderer, glm::uvec2 const& size, IShaderProgram& combineShader) :
        _renderer(renderer),
        _combineShader(combineShader),
        _quad(renderer)
    {
        this->_gbufferRenderTarget = this->_renderer.CreateRenderTarget(size);
        this->_gbufferRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba16f, Tools::Renderers::RenderTargetUsage::Color); // Colors
        this->_gbufferRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba16f, Tools::Renderers::RenderTargetUsage::Color); // Normals (spherical) + Depth + Material
        this->_gbufferRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Depth24Stencil8, Tools::Renderers::RenderTargetUsage::DepthStencil); // Z Buffer
        this->_lightRenderTarget = this->_renderer.CreateRenderTarget(size);
        this->_lightRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba8, Tools::Renderers::RenderTargetUsage::Color); // Light data
        this->_lightRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba8, Tools::Renderers::RenderTargetUsage::Color); // Specular data

        this->_quadModelViewProj = this->_combineShader.GetParameter("quadWorldViewProjection");
        this->_diffuseTexture = this->_combineShader.GetParameter("diffuse");
        this->_lightTexture = this->_combineShader.GetParameter("lighting");
        this->_specularTexture = this->_combineShader.GetParameter("specular");

        glm::fvec2 sizef(size);
        this->_mvp = glm::ortho(0.0f, sizef.x, sizef.y, 0.0f)
            * glm::scale(sizef.x, sizef.y, 1.0f)
            * glm::translate(0.5f, 0.5f, 1.0f);
    }

    void GBuffer::Resize(glm::uvec2 const& size)
    {
        this->_gbufferRenderTarget->Resize(size);
        this->_lightRenderTarget->Resize(size);

        glm::fvec2 sizef(size);
        this->_mvp = glm::ortho(-0.5f, 0.5f, 0.5f, -0.5f) * glm::translate(0.0f, 0.0f, 1.0f);
    }

    void GBuffer::Bind()
    {
        this->_renderer.BeginDraw(this->_gbufferRenderTarget.get());
    }

    void GBuffer::Unbind()
    {
        this->_renderer.EndDraw();
    }

    void GBuffer::BeginLighting()
    {
        this->_renderer.BeginDraw(this->_lightRenderTarget.get());
    }

    void GBuffer::EndLighting()
    {
        this->_renderer.EndDraw();
    }

    void GBuffer::Render()
    {
        auto size = glm::fvec2(this->_gbufferRenderTarget->GetSize());

        this->_quadModelViewProj->Set(this->_mvp, true);
        do
        {
            this->_combineShader.BeginPass();

            this->GetLighting().Bind();
            this->_lightTexture->Set(this->GetLighting());
            this->GetSpecular().Bind();
            this->_specularTexture->Set(this->GetSpecular());
            this->_quad.Render(*this->_diffuseTexture, this->GetColors());
            this->GetSpecular().Unbind();
            this->GetLighting().Unbind();

        } while (this->_combineShader.EndPass());
    }

}}}
