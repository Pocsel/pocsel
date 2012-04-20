#include "tools/precompiled.hpp"
#include "tools/renderers/utils/GBuffer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    GBuffer::GBuffer(IRenderer& renderer, glm::uvec2 const& size) :
        _renderer(renderer)
    {
        this->_renderTarget = this->_renderer.CreateRenderTarget(size);
        this->_renderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba8, Tools::Renderers::RenderTargetUsage::Color); // Colors
        this->_renderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba16f, Tools::Renderers::RenderTargetUsage::Color); // Normals
        this->_renderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Rgba16f, Tools::Renderers::RenderTargetUsage::Color); // Positions XYZ
        this->_renderTarget->PushRenderTarget(Tools::Renderers::PixelFormat::Depth24Stencil8, Tools::Renderers::RenderTargetUsage::DepthStencil); // Z Buffer
    }

    void GBuffer::Bind()
    {
        this->_renderer.BeginDraw(this->_renderTarget.get());
    }

    void GBuffer::Unbind()
    {
        this->_renderer.EndDraw();
    }

}}}
