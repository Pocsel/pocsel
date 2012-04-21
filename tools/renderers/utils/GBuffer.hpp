#ifndef __TOOLS_RENDERERS_UTILS_GBUFFER_HPP__
#define __TOOLS_RENDERERS_UTILS_GBUFFER_HPP__

#include "tools/IRenderer.hpp"

namespace Tools { namespace Renderers { namespace Utils {

    class GBuffer
    {
    private:
        IRenderer& _renderer;
        std::unique_ptr<IRenderTarget> _renderTarget;

    public:
        GBuffer(IRenderer& renderer, glm::uvec2 const& size);

        void Resize(glm::uvec2 const& size) { this->_renderTarget->Resize(size); }

        void Bind();
        void Unbind();

        ITexture2D& GetColors() { return this->_renderTarget->GetTexture(0); }
        ITexture2D& GetNormals() { return this->_renderTarget->GetTexture(1); }
        ITexture2D& GetPositions() { return this->_renderTarget->GetTexture(2); }
        ITexture2D& GetDepth() { return this->_renderTarget->GetTexture(3); }
    };

}}}

#endif
