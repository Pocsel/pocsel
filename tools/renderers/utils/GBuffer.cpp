#include "tools/precompiled.hpp"
#include "tools/renderers/utils/GBuffer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    GBuffer::GBuffer(IRenderer& renderer, glm::uvec2 const& size) :
        _renderer(renderer)
    {
        this->_gbufferRenderTarget = this->_renderer.CreateRenderTarget(size);
        this->_gbufferRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba8, Tools::Renderers::RenderTargetUsage::Color); // Colors
        this->_gbufferRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rg16f, Tools::Renderers::RenderTargetUsage::Color); // Normals
        this->_gbufferRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgb10a2, Tools::Renderers::RenderTargetUsage::Color); // Positions XYZ
        this->_gbufferRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Depth24Stencil8, Tools::Renderers::RenderTargetUsage::DepthStencil); // Z Buffer
        this->_lightRenderTarget = this->_renderer.CreateRenderTarget(size);
        this->_lightRenderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba8, Tools::Renderers::RenderTargetUsage::Color); // Light data
    }

    void GBuffer::Resize(glm::uvec2 const& size)
    {
        this->_gbufferRenderTarget->Resize(size);
        this->_lightRenderTarget->Resize(size);
    }

    void GBuffer::Bind()
    {
        this->_renderer.BeginDraw(this->_gbufferRenderTarget.get());
    }

    void GBuffer::Unbind()
    {
        this->_renderer.EndDraw();
    }

    void GBuffer::BindLighting()
    {
        this->_renderer.BeginDraw(this->_lightRenderTarget.get());
    }

    void GBuffer::UnbindLighting()
    {
        this->_renderer.EndDraw();
    }

}}}
