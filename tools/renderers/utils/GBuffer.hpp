#ifndef __TOOLS_RENDERERS_UTILS_GBUFFER_HPP__
#define __TOOLS_RENDERERS_UTILS_GBUFFER_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class GBuffer
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IRenderTarget> _gbufferRenderTarget;
        std::unique_ptr<IRenderTarget> _lightRenderTarget;

    public:
        GBuffer(IRenderer& renderer, glm::uvec2 const& size);

        void Resize(glm::uvec2 const& size);

        void Bind();
        void Unbind();

        void BindLighting();
        void UnbindLighting();

        ITexture2D& GetColors() { return this->_gbufferRenderTarget->GetTexture(0); }
        ITexture2D& GetNormals() { return this->_gbufferRenderTarget->GetTexture(1); }
        ITexture2D& GetPositions() { return this->_gbufferRenderTarget->GetTexture(2); }
        ITexture2D& GetDepth() { return this->_gbufferRenderTarget->GetTexture(3); }
        ITexture2D& GetLighting() { return this->_lightRenderTarget->GetTexture(0); }
    };

}}}

#endif
