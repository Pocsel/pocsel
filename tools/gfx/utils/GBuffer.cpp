#include "tools/precompiled.hpp"
#include "tools/gfx/utils/GBuffer.hpp"
#include "tools/gfx/utils/material/LuaMaterial.hpp"
#include "tools/gfx/utils/texture/ITexture.hpp"
#include "tools/gfx/utils/texture/Texture.hpp"

namespace Tools { namespace Gfx { namespace Utils {

    namespace {
        unsigned int _NextPowerOf2(unsigned int n)
        {
            --n;
            n = n | (n >> 1);
            n = n | (n >> 2);
            n = n | (n >> 4);
            n = n | (n >> 8);
            n = n | (n >> 16);
            return ++n;
        }
    }

    GBuffer::GBuffer(IRenderer& renderer, glm::uvec2 const& size, Effect::Effect& combineShader) :
        _renderer(renderer),
        _combineShader(combineShader),
        _quad(renderer),
        _size(size)
    {
        this->_gbufferRenderTarget = this->_renderer.CreateRenderTarget(size);
        this->_gbufferRenderTarget->PushRenderTarget(PixelFormat::Rgba16f, RenderTargetUsage::Color); // Colors
        this->_gbufferRenderTarget->PushRenderTarget(PixelFormat::Rgba16f, RenderTargetUsage::Color); // Normals (spherical) + Depth + Material
        this->_gbufferRenderTarget->PushRenderTarget(PixelFormat::Depth24Stencil8, RenderTargetUsage::DepthStencil); // Z Buffer
        this->_lightRenderTarget = this->_renderer.CreateRenderTarget(size);
        this->_lightRenderTarget->PushRenderTarget(PixelFormat::Rgba8, RenderTargetUsage::Color); // Light data
        this->_lightRenderTarget->PushRenderTarget(PixelFormat::Rgba8, RenderTargetUsage::Color); // Specular data
        this->_finalRenderTarget = this->_renderer.CreateRenderTarget(size);
        this->_finalRenderTarget->PushRenderTarget(PixelFormat::Rgba8, RenderTargetUsage::Color); // Final image without postprocessing

        this->_quadModelViewProj = &this->_combineShader.GetParameter("quadWorldViewProjection");
        this->_diffuseTexture = &this->_combineShader.GetParameter("diffuse");
        this->_lightTexture = &this->_combineShader.GetParameter("lighting");
        this->_specularTexture = &this->_combineShader.GetParameter("specular");

        this->_mvp = glm::ortho(-0.5f, 0.5f, 0.5f, -0.5f) * glm::translate(0.0f, 0.0f, 1.0f);
    }

    void GBuffer::Resize(glm::uvec2 const& size)
    {
        this->_size = size;
        this->_gbufferRenderTarget->Resize(size);
        this->_lightRenderTarget->Resize(size);
        this->_finalRenderTarget->Resize(size);

        //this->_mvp = glm::ortho(-0.5f, 0.5f, 0.5f, -0.5f) * glm::translate(0.0f, 0.0f, 1.0f);
    }

    void GBuffer::Bind()
    {
        this->_renderer.BeginDraw(this->_gbufferRenderTarget.get());
        this->_renderer.SetViewport(glm::uvec2(0), this->_size);
    }

    void GBuffer::Unbind()
    {
        this->_renderer.EndDraw();
    }

    void GBuffer::BeginLighting()
    {
        this->_renderer.BeginDraw(this->_lightRenderTarget.get());
        this->_renderer.SetViewport(glm::uvec2(0), this->_size);
    }

    void GBuffer::EndLighting()
    {
        this->_renderer.EndDraw();
    }

    void GBuffer::Render(Uint64 totalTime, std::list<Material::LuaMaterial*> const& postProcess)
    {
        auto& colors = this->GetColors();
        auto& normalDepth = this->GetNormalsDepth();
        auto& lighting = this->GetLighting();
        auto& specular = this->GetSpecular();
        auto& finalImg = this->GetFinal();

        // Combine lighting with albedo
        if (!postProcess.empty())
            this->_renderer.BeginDraw(this->_finalRenderTarget.get());
        this->_renderer.SetDepthTest(false);
        this->_renderer.SetCullMode(Tools::Gfx::CullMode::None);
        colors.Bind();
        lighting.Bind();
        specular.Bind();
        this->_diffuseTexture->Set(colors);
        this->_lightTexture->Set(this->GetLighting());
        this->_specularTexture->Set(this->GetSpecular());
        this->_quadModelViewProj->Set(this->_mvp, true);
        do
        {
            this->_combineShader.BeginPass();
            this->_quad.Render();
        } while (this->_combineShader.EndPass());
        colors.Unbind();
        lighting.Unbind();
        specular.Unbind();

        if (postProcess.empty())
            return;
        this->_renderer.EndDraw();

        this->_renderer.SetDepthTest(false);
        //this->_renderer.SetCullMode(CullMode::None);
        // Post processing shaders
        finalImg.Bind();
        normalDepth.Bind();
        for (auto it = postProcess.begin(), ite = postProcess.end(); it != ite; ++it)
        {
            auto& material = (*it)->GetMaterial();
            auto& shader = material.GetGeometryShader();
            material.Update(totalTime);
            material.UpdateParameters(shader, totalTime);
            shader.GetParameter("quadWorldViewProjection").Set(this->_mvp, true);
            shader.GetParameter("diffuse").Set(finalImg);
            shader.GetParameter("normalDepth").Set(normalDepth);
            do
            {
                shader.BeginPass();
                this->_quad.Render();
            } while (shader.EndPass());
        }
        finalImg.Unbind();
        normalDepth.Unbind();
    }

}}}
